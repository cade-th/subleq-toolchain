{-# LANGUAGE OverloadedStrings #-}
module Main where

import Subleq.Assembler
import Subleq.Emulator
import Data.Aeson
import qualified Data.ByteString.Lazy as B
import qualified Data.Map as Map
import System.Exit (exitFailure)
import Data.Text (Text)
import qualified Data.Text as T
import qualified Data.Vector as V

data E2ETestCase = E2ETestCase {
    inputSource :: String,
    e2eExpectedOutput :: String,
    e2eExpectedPC :: Int,
    e2eMaxCycles :: Int,
    e2eExpectedMemory :: Map.Map String Int
} deriving (Show)

instance FromJSON E2ETestCase where
    parseJSON = withObject "E2ETestCase" $ \v -> E2ETestCase
        <$> v .: "input"
        <*> v .:? "expected_output" .!= ""
        <*> v .:? "expected_pc" .!= 0
        <*> v .:? "max_cycles" .!= 1000
        <*> v .:? "expected_memory" .!= Map.empty

runE2ETest :: (String, E2ETestCase) -> IO Bool
runE2ETest (name, tc) = do
    case assemble (inputSource tc) of
        Left err -> do
            putStrLn $ "E2E Assemble error in " ++ name ++ ": " ++ err
            return False
        Right binary -> do
            let cpu = run (e2eMaxCycles tc) (newCPU binary)
            let outMatch = cpuOutput cpu == e2eExpectedOutput tc
            let pcMatch = cpuPC cpu == e2eExpectedPC tc
            let memMatch = all (\(addrStr, expVal) -> 
                                   let addr = read addrStr :: Int
                                   in (cpuMemory cpu V.! addr) == expVal
                               ) (Map.toList (e2eExpectedMemory tc))
            
            if outMatch && pcMatch && memMatch
                then return True
                else do
                    putStrLn $ "E2E mismatch in " ++ name
                    return False

main :: IO ()
main = do
    content <- B.readFile "tests/end_to_end/end_to_end_data.json"
    case decode content :: Maybe (Map.Map String E2ETestCase) of
        Nothing -> do
            putStrLn "Failed to parse end_to_end_data.json"
            exitFailure
        Just d -> do
            results <- mapM runE2ETest (Map.toList d)
            if all id results
                then putStrLn "End-to-End Tests: PASS"
                else do
                    putStrLn "End-to-End Tests: FAIL"
                    exitFailure
