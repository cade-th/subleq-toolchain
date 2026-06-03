{-# LANGUAGE OverloadedStrings #-}
module Main where

import Subleq.Lexer
import Data.Aeson
import qualified Data.ByteString.Lazy as B
import qualified Data.Map as Map
import System.Exit (exitFailure)
import Data.Text (Text)
import qualified Data.Text as T

data LexerTestCase = LexerTestCase {
    input :: String,
    expected :: [TokenJSON]
} deriving (Show)

data TokenJSON = TokenJSON {
    tjType :: String,
    tjValue :: String
} deriving (Show)

instance FromJSON LexerTestCase where
    parseJSON = withObject "LexerTestCase" $ \v -> LexerTestCase
        <$> v .: "input"
        <*> v .: "expected"

instance FromJSON TokenJSON where
    parseJSON = withObject "TokenJSON" $ \v -> TokenJSON
        <$> v .: "type"
        <*> v .: "value"

matchToken :: Token -> TokenJSON -> Bool
matchToken (Token t v) (TokenJSON ejT ejV) =
    show t == ejT && v == ejV

runTest :: (String, LexerTestCase) -> IO Bool
runTest (name, tc) = do
    case tokenize (input tc) of
        Left err -> do
            putStrLn $ "Lexer error in " ++ name ++ ": " ++ show err
            return False
        Right output -> do
            let expectedTokens = expected tc
            if length output /= length expectedTokens
                then do
                    putStrLn $ "Token count mismatch in " ++ name ++ ": Expected " ++ show (length expectedTokens) ++ ", got " ++ show (length output)
                    return False
                else do
                    let matches = zipWith matchToken output expectedTokens
                    if all id matches
                        then return True
                        else do
                            putStrLn $ "Token mismatch in " ++ name
                            return False

main :: IO ()
main = do
    content <- B.readFile "tests/assembler/lexer_data.json"
    case decode content :: Maybe (Map.Map String LexerTestCase) of
        Nothing -> do
            putStrLn "Failed to parse lexer_data.json"
            exitFailure
        Just testData -> do
            results <- mapM runTest (Map.toList testData)
            if all id results
                then putStrLn "Lexer Tests: PASS"
                else do
                    putStrLn "Lexer Tests: FAIL"
                    exitFailure
