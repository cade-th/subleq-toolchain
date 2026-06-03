module Subleq.Emulator where

import Data.Char (chr)
import System.IO (hPutStr, stderr, stdout, hFlush)
import Data.Vector (Vector, (!), (//))
import qualified Data.Vector as V

data CPU = CPU {
    cpuPC :: Int,
    cpuMemory :: Vector Int,
    cpuTotalCycles :: Int,
    cpuOutput :: String
} deriving (Show, Eq)

newCPU :: [Int] -> CPU
newCPU mem = CPU {
    cpuPC = 0,
    cpuMemory = V.fromList mem,
    cpuTotalCycles = 0,
    cpuOutput = ""
}

subleq :: CPU -> CPU
subleq cpu
    | cpuPC cpu < 0 || cpuPC cpu + 2 >= V.length (cpuMemory cpu) = cpu
    | otherwise =
        let m = cpuMemory cpu
            pc = cpuPC cpu
            aAddr = m ! pc
            bAddr = m ! (pc + 1)
            cAddr = m ! (pc + 2)
        in if bAddr == -1
           then let valA = m ! aAddr
                    newOutput = cpuOutput cpu ++ [chr valA]
                    nextPC = if -valA <= 0 then cAddr else pc + 3
                in cpu { cpuPC = nextPC, cpuTotalCycles = cpuTotalCycles cpu + 1, cpuOutput = newOutput }
           else let valA = m ! aAddr
                    valB = m ! bAddr
                    res = valB - valA
                    newMem = m // [(bAddr, res)]
                    nextPC = if res <= 0 then cAddr else pc + 3
                in cpu { cpuPC = nextPC, cpuMemory = newMem, cpuTotalCycles = cpuTotalCycles cpu + 1 }

run :: Int -> CPU -> CPU
run 0 cpu = cpu
run cycles cpu
    | cpuPC cpu < 0 = cpu
    | otherwise = run (cycles - 1) (subleq cpu)
