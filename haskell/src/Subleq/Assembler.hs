{-# LANGUAGE OverloadedStrings #-}
module Subleq.Assembler where

import Subleq.Lexer
import Data.Map (Map)
import qualified Data.Map as Map
import Data.Char (toLower)

data MacroDef = MacroDef {
    macroParams :: [String],
    macroBody :: [Token],
    macroArgc :: Int
} deriving (Show, Eq)

data AssemblerState = AssemblerState {
    macros :: Map String MacroDef,
    symbols :: Map String Int,
    pc :: Int,
    tokens :: [Token]
} deriving (Show, Eq)

data Pass1Result = Pass1Result {
    resMacros :: Map String MacroDef,
    resSymbols :: Map String Int,
    resTokens :: [Token],
    resPC :: Int
} deriving (Show, Eq)

parseDefArgs :: [Token] -> Int -> ([String], Int)
parseDefArgs ts pos
    | pos < length ts && tokenType (ts !! pos) == LPAREN = 
        let (args, nextPos) = collectArgs (pos + 1)
        in (args, nextPos)
    | otherwise = ([], pos)
  where
    collectArgs p
        | p >= length ts || tokenType (ts !! p) == RPAREN = ([], p + 1)
        | tokenType (ts !! p) == SYMBOL = 
            let (rest, lastPos) = collectArgs (p + 1)
            in (tokenValue (ts !! p) : rest, lastPos)
        | tokenType (ts !! p) == COMMA = collectArgs (p + 1)
        | otherwise = collectArgs (p + 1)

collectCallArgs :: [Token] -> Int -> Int -> ([[Token]], Int)
collectCallArgs ts pos argc
    | pos < length ts && tokenType (ts !! pos) == LPAREN =
        let (args, nextPos) = collect (pos + 1) 0
        in (args, nextPos)
    | otherwise = ([], pos)
  where
    collect p count
        | count >= argc =
            if p < length ts && tokenType (ts !! p) == RPAREN
            then ([], p + 1)
            else ([], p)
        | p >= length ts = ([], p)
        | otherwise = 
            let (arg, nextP) = collectArg p
                (rest, lastP) = collect nextP (count + 1)
            in (arg : rest, lastP)
    collectArg p
        | p >= length ts || tokenType (ts !! p) == COMMA || tokenType (ts !! p) == RPAREN = ([], p)
        | otherwise = 
            let (rest, nextP) = collectArg (p + 1)
            in (ts !! p : rest, nextP)

substitute :: [Token] -> [String] -> [[Token]] -> [Token]
substitute body params args = concatMap substToken body
  where
    substToken t
        | tokenType t == SYMBOL =
            case Map.lookup (tokenValue t) paramMap of
                Just idx -> args !! idx
                Nothing -> [t]
        | otherwise = [t]
    paramMap = Map.fromList $ zip params [0..]

expand :: [Token] -> Map String MacroDef -> Map String Int -> [Token] -> Int -> (Int, [Token], Map String MacroDef, Map String Int)
expand ts ms ss out c = go 0 c [] ms ss
  where
    go i currPC acc macros symbols
        | i >= length ts = (currPC, reverse acc, macros, symbols)
        | tokenType (ts !! i) == HASH && (i + 1) < length ts && tokenValue (ts !! (i + 1)) == "define" =
            let name = tokenValue (ts !! (i + 2))
                (params, nextPosInitial) = parseDefArgs ts (i + 3)
                (body, nextPos) = collectMacroBody nextPosInitial
                newMacros = Map.insert name (MacroDef params body (length params)) macros
            in go nextPos currPC acc newMacros symbols
        | tokenType (ts !! i) == LABEL =
            let newSymbols = Map.insert (tokenValue (ts !! i)) currPC symbols
            in go (i + 1) currPC acc macros newSymbols
        | tokenType (ts !! i) == SYMBOL && Map.member (tokenValue (ts !! i)) macros =
            let macro = macros Map.! tokenValue (ts !! i)
                (args, nextPos) = collectCallArgs ts (i + 1) (macroArgc macro)
                hasParens = (i + 1 < length ts) && tokenType (ts !! (i + 1)) == LPAREN
            in if hasParens || macroArgc macro == 0
               then let substituted = substitute (macroBody macro) (macroParams macro) args
                        (expandedPC, expandedTokens, newMacros, newSymbols) = expand substituted macros symbols [] currPC
                    in go nextPos expandedPC (reverse expandedTokens ++ acc) newMacros newSymbols
               else go (i + 1) (currPC + 1) (ts !! i : acc) macros symbols
        | tokenType (ts !! i) == SYMBOL || tokenType (ts !! i) == NUMBER =
            go (i + 1) (currPC + 1) (ts !! i : acc) macros symbols
        | otherwise = go (i + 1) currPC acc macros symbols

    collectMacroBody pInitial = 
        let (body, nextP) = goCollect pInitial
        in (body, nextP)
      where
        goCollect p
            | p >= length ts = ([], p)
            | tokenType (ts !! p) == SYMBOL && map toLower (tokenValue (ts !! p)) == "endm" = ([], p + 1)
            | otherwise = 
                let (rest, lastP) = goCollect (p + 1)
                in (ts !! p : rest, lastP)

pass1 :: [Token] -> Pass1Result
pass1 ts = 
    let (finalPC, expandedTokens, finalMacros, finalSymbols) = expand ts Map.empty Map.empty [] 0
    in Pass1Result finalMacros finalSymbols expandedTokens finalPC

assemble :: String -> Either String [Int]
assemble source = do
    case tokenize source of
        Left err -> Left (show err)
        Right ts -> 
            let res = pass1 ts
                results = map (resolve res) (resTokens res)
            in sequence results
  where
    resolve res t
        | tokenType t == SYMBOL =
            case Map.lookup (tokenValue t) (resSymbols res) of
                Just addr -> Right addr
                Nothing -> Left $ "Undefined label: " ++ tokenValue t
        | tokenType t == NUMBER =
            Right $ floor (read (tokenValue t) :: Double)
        | otherwise = Left "Unexpected token during resolution"
