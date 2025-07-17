# c2p2

- Data used: https://opendata.cern.ch/record/80001, index "data16_13TeV_Run_00296939_file_index.json", files "DAOD_PHYSLITE.37019878"
- Benchmark results: https://drive.google.com/drive/folders/1j3q4KMjq993xRYTdGZJgh_YUyHmxyMK5?usp=drive_link


## Build instructions
- First/fresh build
    - In repo root, `mkdir bin`
    - `cd bin`
    - `cmake ../src`
    - `make`
- Subsequent builds
    - In `bin`, run `make`

## Benchmarking instructions
- Use scripts in `scripts/`
- Scripts should write results to `benchmark results/`
    - CSV files -- Benchmark data
    - ROOT files -- Decompressed data produced during benchmark
    - `.log` files -- Statements printed during benchmark
- Scripts expect that source data is in `../data` -- i.e. a directory `data/` that exists at the same level as `scripts/`
