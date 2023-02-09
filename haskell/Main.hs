module Main where

import Options.Applicative
import Data.Semigroup ((<>))

data Mode = Generate GenOptions | Encrypt EncOptions | Decrypt DecOption

data Options = Options
    { optMode :: Mode
    , }

modes :: Parser (IO ())
modes = subparser
  ( command "add" (info addOptions ( progDesc "Add a file to the repository" ))
 <> command "commit" (info commitOptions ( progDesc "Record changes to the repository" ))
  )

options :: Parser Options
options = Options
      <$> strOption
          ( long "hello"
         <> metavar "TARGET"
         <> help "Target for the greeting" )
      <*> switch
          ( long "quiet"
         <> short 'q'
         <> help "Whether to be quiet" )
      <*> option auto
          ( long "enthusiasm"
         <> help "How enthusiastically to greet"
         <> showDefault
         <> value 1
         <> metavar "INT" )

main :: IO ()
main = greet =<< execParser opts
  where
    opts = info (sample <**> helper)
      ( fullDesc
     <> progDesc "Print a greeting for TARGET"
     <> header "hello - a test for optparse-applicative" )

run :: Options -> IO ()
run (Sample h False n) = putStrLn $ "Hello, " ++ h ++ replicate n '!'
run _ = return ()