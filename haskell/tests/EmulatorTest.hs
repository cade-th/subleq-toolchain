{-# LANGUAGE OverloadedStrings #-}
module Main where

import Subleq.Emulator
import Data.Aeson
import qualified Data.ByteString.Lazy as B
import qualified Data.Map as Map
import System.Exit (exitFailure)
import Data.Text (Text)
import qualified Data.Text as T
import qualified Data.Vector as V

data EmuTestCase = EmuTestCase {
    program :: [Int],
    expectedOutput :: String,
    expectedPC :: Int,
    maxCycles :: Int,
    expectedMemory :: Map.Map String Int
} deriving (Show)

instance FromJSON EmuTestCase where
    parseJSON = withObject "EmuTestCase" $ \v -> EmuTestCase
        <$> v .: "program"
        <*> v .:? "expected_output" .!= ""
        <*> v .:? "expected_pc" .!= 0
        <*> v .:? "max_cycles" .!= 100
        <*> v .:? "expected_memory" .!= Map.empty

runEmuTest :: (String, EmuTestCase) -> IO Bool
runEmuTest (name, tc) = do
    let cpu = run (maxCycles tc) (newCPU (program tc))
    let outMatch = cpuOutput cpu == expectedOutput tc
    let pcMatch = cpuPC cpu == expectedPC tc
    let memMatch = all (\(addrStr, expVal) -> 
                           let addr = read addrStr :: Int
                           in (cpuMemory cpu V.! addr) == expVal
                       ) (Map.toList (expectedMemory tc))
    
    if outMatch && pcMatch && memMatch
        then return True
        else do
            putStrLn $ "Emulator mismatch in " ++ name
            return False

main :: IO ()
main = do
    content <- B.readFile "tests/emu/emu_test_data.json"
    case decode content :: Maybe (Map.Map String EmuTestCase) of
        Nothing -> do
            putStrLn "Failed to parse emu_test_data.json"
            exitFailure
        Just d -> do
            results <- mapM runEmuTest (Map.toList d)
            if all id results
                then putStrLn "Emulator Tests: PASS"
                else do
                    putStrLn "Emulator Tests: FAIL"
                    exitFailure
