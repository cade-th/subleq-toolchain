{-# LANGUAGE OverloadedStrings #-}
module Main where

import Subleq.Assembler
import Subleq.Lexer
import Data.Aeson
import qualified Data.ByteString.Lazy as B
import qualified Data.Map as Map
import System.Exit (exitFailure)
import Data.Text (Text)
import qualified Data.Text as T

data Pass1TestCase = Pass1TestCase {
    p1Input :: String,
    p1Expected :: Pass1Expected
} deriving (Show)

data Pass1Expected = Pass1Expected {
    expPC :: Int,
    expSymbols :: Map.Map String Int,
    expMacros :: Map.Map String MacroExpected
} deriving (Show)

data MacroExpected = MacroExpected {
    expArgc :: Int
} deriving (Show)

instance FromJSON Pass1TestCase where
    parseJSON = withObject "Pass1TestCase" $ \v -> Pass1TestCase
        <$> v .: "input"
        <*> v .: "expected"

instance FromJSON Pass1Expected where
    parseJSON = withObject "Pass1Expected" $ \v -> Pass1Expected
        <$> v .: "pc"
        <*> v .:? "symbols" .!= Map.empty
        <*> v .:? "macros" .!= Map.empty

instance FromJSON MacroExpected where
    parseJSON = withObject "MacroExpected" $ \v -> MacroExpected
        <$> v .: "argc"

data Pass2TestCase = Pass2TestCase {
    p2Input :: String,
    p2Expected :: [Int]
} deriving (Show)

instance FromJSON Pass2TestCase where
    parseJSON = withObject "Pass2TestCase" $ \v -> Pass2TestCase
        <$> v .: "input"
        <*> v .: "expected"

runPass1Test :: (String, Pass1TestCase) -> IO Bool
runPass1Test (name, tc) = do
    case tokenize (p1Input tc) of
        Left err -> do
            putStrLn $ "Pass1 Tokenize error in " ++ name ++ ": " ++ show err
            return False
        Right ts -> do
            let res = pass1 ts
            let exp = p1Expected tc
            let pcMatch = resPC res == expPC exp
            let symMatch = resSymbols res == expSymbols exp
            let macrosMatch = all (\(mName, mExp) -> 
                                    case Map.lookup mName (resMacros res) of
                                        Just m -> macroArgc m == expArgc mExp
                                        Nothing -> False
                                ) (Map.toList (expMacros exp))
            if pcMatch && symMatch && macrosMatch
                then return True
                else do
                    putStrLn $ "Pass1 mismatch in " ++ name
                    return False

runPass2Test :: (String, Pass2TestCase) -> IO Bool
runPass2Test (name, tc) = do
    case assemble (p2Input tc) of
        Left err -> do
            putStrLn $ "Pass2 assemble error in " ++ name ++ ": " ++ err
            return False
        Right binary -> do
            if binary == p2Expected tc
                then return True
                else do
                    putStrLn $ "Pass2 binary mismatch in " ++ name
                    return False

main :: IO ()
main = do
    p1content <- B.readFile "tests/assembler/pass1_data.json"
    p2content <- B.readFile "tests/assembler/pass2_data.json"
    
    let p1Data = decode p1content :: Maybe (Map.Map String Pass1TestCase)
    let p2Data = decode p2content :: Maybe (Map.Map String Pass2TestCase)
    
    case (p1Data, p2Data) of
        (Just d1, Just d2) -> do
            r1 <- mapM runPass1Test (Map.toList d1)
            r2 <- mapM runPass2Test (Map.toList d2)
            if all id r1 && all id r2
                then putStrLn "Assembler Tests: PASS"
                else do
                    putStrLn "Assembler Tests: FAIL"
                    exitFailure
        _ -> do
            putStrLn "Failed to parse assembler test JSON files"
            exitFailure
