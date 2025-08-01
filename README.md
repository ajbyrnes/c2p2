# c2p2

- Aliases I have been using for ATLAS data stored:
	- data
	- 3G-data
	- 13G-data
	- 134G-data

- Results get written to `benchmark results`, which should not exist in the repo ever
- `analysis` contains notebooks and possibly data
	- How to manage my output data and analysis notebooks is an open problem
	- I am trying to avoid getting the repo clogged up esp with notebooks
	- As a rule, `.root`, `.csv`, and `.ipynb` are excluded from the repo unless forced in

- `src/benchmarking` contains code for running benchmarks using the `CompressorBenchmark`, `TruncCompressor`, and `SZ3Compressor` classes
	- A **benchmark program** is an executable compiled from a source file in `src/benchmarking` with the form `benchmark-*.cpp`
	- Benchmark program logic:
		- *Parse command line arguments*
		- *Set names of output files `outputCSV` and `outputROOT`*
		- *Iterate over specified branches*
			- *Load data from specified ROOT file*
			- *Iterate over compressor parameters*
				- This loop structure minimizes the number of times we have to load data.
				- *Instantiate `Compressor`*
				- *Instantiate `CompressorBenchmark` which receives 1) the `Compressor`, 2) the data to compress, packaged in a struct along with some metadata, and 3) the name of the file to write benchmark results to, `outputCSV`*
				- *Perform benchmark via `CompressorBenchmark::run()`*
				- *Accumulate decompressed data returned from `CompressorBenchmark::run()`*
			- *Write decompressed data to a TTree in `outputROOT`*
				- This is set up so that if a single run of the benchmark program iterates over `n` branches and `m` configurations of parameters, you end up with a ROOT file containing `n` TTrees with `m + 1 branches each. 
					- A TTree corresponds to a single set of data getting compressed
					- `m` branch in the TTree corresponds to the decompressed data resulting from a set of compression parameters 
					- The last branch in each TTree is the original, uncompressed data, for ease of access when performing later analysis
				- I don't think this is particularly scalable, as it requires all decompressed data to remain in memory until we've looked at all the parameter combinations for a given branch
					- The reason it's done this way right now is that this is basically an abuse of TTrees and decompressed data should probably be written to another format
					- TTrees are designed for efficiently collecting *sequences of events*. If you create a new TTree and write one branch `a` with 10 entries, this is equivalent to recording quantity `a` for 10 events. If you then open the same TTree and write a new branch `b` with 10 entries, you are recording quantity `b` for 10 *additional* events. That is, you do *not* now have a rectangle of data with indices 0,...,9 for columns `a` and `b`. You have a rectangle of data with indices 0,...,19, where column `b` is empty for the first 10 indices, and column `a` is empty for the last 10 indices
					- TL;DR If we have dataset `A` and decompressed data `A'`, and `A''`, and we want all of these datasets to share an index, then they need to be written into the TTree *at the same time*
	- `CompressorBenchmark::run()` logic:
		- *Repeat N times:*
			- *Compress and decompress data*
			- *Compute stats -- compression ratio, error, timing, etc.*
				- I'm calculating more than I need right now, but nowhere near enough to be a problem compared to the total time to compress/decompress, especially at larger data sizes.)
				- The algorithms are 100% deterministic, the only reason to iterate a couple times over a single blob of data is to average out the compression time and decompression time)
			- *Write stats out as a CSV line -- the file this is written to is specified when the benchmark is created*
				- Yes, gross, CSV, but I don't have the fortitude to add another library to this right now)
		- *Return decompressed data so it can be written to a ROOT file and we can see how well the distribution has been preserved*

- Build process
	- `mkdir bin` or `mkdir build` or whatever your favorite build directory name is
	- `cmake ../src`
	- `make`

- Run process
	- Go to `scripts` and run scripts from there
	- Output should get written to `benchmark results`; from there it can be moved to a better home
