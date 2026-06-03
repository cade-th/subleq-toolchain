{-# LANGUAGE OverloadedStrings #-}
module Subleq.Lexer where

import Text.Megaparsec
import Text.Megaparsec.Char
import qualified Text.Megaparsec.Char.Lexer as L
import Data.Text (Text)
import qualified Data.Text as T
import Data.Void

type Parser = Parsec Void Text

data TokenType = NUMBER | HASH | LABEL | SYMBOL | LPAREN | RPAREN | COMMA
  deriving (Show, Eq, Ord)

data Token = Token {
    tokenType :: TokenType,
    tokenValue :: String
} deriving (Show, Eq)

sc :: Parser ()
sc = L.space
  space1
  empty
  empty

lexeme :: Parser a -> Parser a
lexeme = L.lexeme sc

symbol :: Text -> Parser Text
symbol = L.symbol sc

pToken :: Parser Token
pToken = choice [
    pLabel,
    pNumber,
    pHash,
    pLparen,
    pRparen,
    pComma,
    pSymbol
  ]

pNumber :: Parser Token
pNumber = lexeme $ do
    s <- (++) <$> (option "" (string "-")) <*> some (digitChar <|> char '.')
    return $ Token NUMBER s

pHash :: Parser Token
pHash = lexeme $ do
    _ <- char '#'
    return $ Token HASH "#"

pLabel :: Parser Token
pLabel = lexeme $ try $ do
    name <- (:) <$> (letterChar <|> char '_') <*> many (alphaNumChar <|> char '_')
    _ <- char ':'
    return $ Token LABEL name

pSymbol :: Parser Token
pSymbol = lexeme $ do
    name <- (:) <$> (letterChar <|> char '_') <*> many (alphaNumChar <|> char '_')
    return $ Token SYMBOL name

pLparen :: Parser Token
pLparen = lexeme $ do
    _ <- char '('
    return $ Token LPAREN "("

pRparen :: Parser Token
pRparen = lexeme $ do
    _ <- char ')'
    return $ Token RPAREN ")"

pComma :: Parser Token
pComma = lexeme $ do
    _ <- char ','
    return $ Token COMMA ","

tokenize :: String -> Either (ParseErrorBundle Text Void) [Token]
tokenize input = parse (sc *> many pToken <* eof) "" (T.pack input)
