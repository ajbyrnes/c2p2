// https://root.cern/doc/master/hist103__THnSparse__hist_8C.html
// Evaluate performance of THnSparse vs TH1/2/3/nF for different dimensions and bins per dimension.
// Calculate bandwidth for filling and retrieving bin contents, in million entries per second.
// First line = Bandwidth based on CPU time
// Second line = Bandwidth based on real time
// Third line = Fraction of filled bins and memory used by THnSparse vs other histograms

#include <iostream>

#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <THn.h>
#include <THnSparse.h>
#include <TRandom.h>
#include <TStopwatch.h>
#include <TStyle.h>
#include <TSystem.h>

#ifndef INT_MAX
#define INT_MAX std::numeric_limits<int>::max()
#endif

// TTimeHists class declaration
class TTimeHists {
    public: 
        enum EHist {kHIST, kSPARSE, kNUMHIST};
        enum ETime {kREAL, kCPU, kNUMTIME};
    
        TTimeHists(int dim, int bins, long num)
        : _value{nullptr}, _dim(dim), _bins(bins), _num(num),
        _sparse{nullptr}, _hist{nullptr}, _histN{nullptr} {}

        ~TTimeHists();

        bool Run();

        double GetTime(EHist hist, ETime time) const {
            if (time == kREAL) return _time[hist][0];
            return _time[hist][1];
        }

        static void SetDebug(int lvl) { _gDebug = lvl; }

        THnSparse* GetSparse() const { return _sparse; }

    protected:
        void Fill(EHist hist);
        double Check(EHist hist);
        void SetupHist(EHist hist);
        void NextValues();
        void SetupValues();
    
    private:
        double* _value;
        int _dim;
        int _bins;
        long _num;
        double _time[kNUMHIST][kNUMTIME];
        THnSparse* _sparse;
        TH1* _hist;
        THn* _histN;
        static int _gDebug;
};

// Set TTimeHists static variable
int TTimeHists::_gDebug = 0;

// TTimeHists function definition -- Deconstructor
TTimeHists::~TTimeHists() {
    delete[] _value;
    delete _sparse;
    delete _hist;
    delete _histN;
}

// TTimeHists function definition -- Run all tests
bool TTimeHists::Run() {
    // Run all tests with current settings
    // Check identity of content
    double check[kNUMHIST];
    long numPoints[kNUMHIST];

    for (int h{0}; h < kNUMHIST; h++) {
        numPoints[h] = 0;
        
        // Perform setup
        SetupValues();

        try {
            // Time histogram setup
            TStopwatch stopwatch{};
            stopwatch.Start();
            SetupHist((EHist)h);
            stopwatch.Stop();

            // Time histogram filling
            do {
                stopwatch.Start(0);
                Fill((EHist)h);
                // check[h] = Check((EHist)h);
                stopwatch.Stop();

                numPoints[h]++;
            } while (((EHist)h != kSPARSE && stopwatch.RealTime() < 0.1) ||
                    ((EHist)h == kSPARSE) && numPoints[0] > 0 && numPoints[1] < numPoints[0]);
            
            // Record times
            _time[h][0] = (1. * _num * numPoints[h]) / stopwatch.RealTime() / 1e6;
            _time[h][1] = (1. * _num * numPoints[h]) / stopwatch.CpuTime() / 1e6;

            if ((EHist)h == kSPARSE || _time[h][0] > 1e20 || _time[h][1] > 1e20) {
                // Fill more points
                do {
                    stopwatch.Start(0);
                    Fill((EHist)h);
                    Check((EHist)h);
                    stopwatch.Stop();

                    numPoints[h]++;
                } while (stopwatch.RealTime() < 0.1);

                // Record times
                _time[h][0] = (1. * _num * numPoints[h]) / stopwatch.RealTime() / 1e6;
                _time[h][1] = (1. * _num * numPoints[h]) / stopwatch.CpuTime() / 1e6;
            }

            // Cap times
            if (_time[h][0] > 1e20) _time[h][0] = 1e20;
            if (_time[h][1] > 1e20) _time[h][1] = 1e20;
        }
        catch (std::exception &) {
            // Set error statuses
            _time[h][0] = _time[h][1] = -1.;
            check[h] = -1.;
            numPoints[h] = -1.;
        }
    }

    return check[0] == check[1];
}

// TTimeHists function definition -- Setup array to hold values to fill into histograms
void TTimeHists::SetupValues() {
    if (!_value) _value = new double[_dim];
    gRandom->SetSeed(42);
}

// TTimeHists function definition -- Generate next values to fill into histograms
void TTimeHists::NextValues() {
    for (int d{0}; d < _dim; d++) {
        _value[d] = gRandom->Gaus() / 4.;
    }
}

// TTimeHists function definitino -- Histogram setup
void TTimeHists::SetupHist(EHist hist) {
    if (hist == kHIST) {
        switch (_dim) {
            case 1:     // 1D histogram
                _hist = new TH1D("h1", "h1", _bins, -1., 1.); 
                break;
            case 2:     // 2D histogram
                _hist = new TH2D("h2", "h2", _bins, -1., 1., _bins, -1., 1.); 
                break;
            case 3:     // 3D histogram
                _hist = new TH3D("h3", "h3", _bins, -1., 1., _bins, -1., 1., _bins, -1., 1.);
                break;
            default:    // ND histogram
                MemInfo_t meminfo{};
                gSystem->GetMemInfo(&meminfo);

                int size{1};
                // Check if there is enough memory for bins in each dimension
                // size = {1, ..., (_bins + 2)^d}
                for (int d{0}; d <= _dim; d++) {
                    if ((int)(size * sizeof(float) > INT_MAX / _bins + 2) ||
                        (meminfo.fMemFree > 0 && meminfo.fMemFree / 2 < (int)(size * sizeof(float) / 1000 / 1000))) {
                    
                        throw std::bad_alloc();
                    }

                    size *= _bins + 2;
                }

                // Create ND histogram
                int* bins{new int[_dim]};
                double* xmin{new double[_dim]}, * xmax{new double[_dim]};

                for (int d{0}; d < _dim; d++) {
                    bins[d] = _bins;
                    xmin[d] = -1.;
                    xmax[d] = 1.;
                }

                _histN = new THnD("hn", "hn", _dim, bins, xmin, xmax);

                break;
        }
    }
    else {
        // Make sparse histogram
        int* bins{new int[_dim]};
        double* xmin{new double[_dim]}, * xmax{new double[_dim]};

        for (int d{0}; d < _dim; d++) {
            bins[d] = _bins;
            xmin[d] = -1.;
            xmax[d] = 1.;
        } 

        _sparse = new THnSparseD("hs", "hs", _dim, bins, xmin, xmax);
    }
}

// TTimeHists function definition -- Fill histogram
void TTimeHists::Fill(EHist hist) {
    for (long n{0}; n < _num; n++) {
        NextValues();

        if (_gDebug > 1) {      // Debug mode
            std::cout << n << ": fill " << (hist == kHIST ? (_dim < 4 ? "hist" : "arr") : "sparse");
            for (int d{0}; d < _dim; d++) {
                std::cout << "[" << _value[d] << "]";
            }
            std::cout << std::endl;
        }
    
        if (hist == kHIST) {
            switch (_dim) {
                case 1:     // 1D histogram
                    _hist->Fill(_value[0]);
                    break;
                case 2:     // 2D histogram
                    ((TH2D*)_hist)->Fill(_value[0], _value[1]);
                    break;
                case 3:     // 3D histogram
                    ((TH3D*)_hist)->Fill(_value[0], _value[1], _value[2]);
                    break;
                default:    // ND histogram
                    _histN->Fill(_value);
                    break;
            }
        }
        else {
            // Sparse histogram
            _sparse->Fill(_value);
        }
    }
}

// TTimeHists function definition -- Check values
double TTimeHists::Check(EHist hist) {
    // Check content of bins
    double check{0};
    int* x{new int[_dim]};
    memset(x, 0, sizeof(int) * _dim);

    if (hist == kHIST) {
        long idx{0};
        
        long size{1};
        for (int d{0}; d < _dim; d++) size *= (_bins + 2);

        while (x[0] <= _bins + 1) {
            // Get bin content
            double binContent{-1.};
            if (_dim < 4) {     // 1D, 2D, or 3D histogram
                long histIdx{x[0]};
                switch(_dim) {
                    case 2:     // 2D histogram
                        histIdx = _hist->GetBin(x[0], x[1]);
                        break;
                    case 3:     // 3D histogram
                        histIdx = _hist->GetBin(x[0], x[1], x[2]);
                }

                binContent = _hist->GetBinContent(histIdx);
            }
            else {      // ND histogram
                binContent = _histN->GetBinContent(x);
            }

            double checkx{0.};
            if (binContent) {
                for (int d{0}; d < _dim; d++) {
                    checkx += x[d];
                }
            }
            check += checkx * binContent;

            if (_gDebug > 2 || (_gDebug > 1 && binContent)) {
                std::cout << (_dim < 4 ? "hist" : "arr") << _dim;
                for (int d{0}; d < _dim; d++) {
                    std::cout << "[" << x[d] << "]";
                }
                std::cout << " = " << binContent << std::endl;
            }

            ++x[_dim - 1];

            // Adjust bin index -- No wrapping for dim 0
            for (int d{_dim - 1}; d > 0; --d) {
                if (x[d] > _bins + 1) {
                    x[d] = 0;
                    ++x[d - 1];
                }
            }

            ++idx;
        }       // end bin loop
    }
    else {
        for (long i{0}; i < _sparse->GetNbins(); i++) {
            double binContent{_sparse->GetBinContent(i, x)};
            double checkx{0.};
            for (int d{0}; d < _dim; d++) {
                checkx += x[d];
            }
            checkx += checkx * binContent;

            if (_gDebug > 1) {
                std::cout << "sparse" << _dim;
                for (int d{0}; d < _dim; d++) {
                    std::cout << "[" << x[d] << "]";
                }
                std::cout << std::endl;
            }
        }
    }

    check /= _num;

    if (gDebug > 0) {
        std::cout << "check " << (hist == kHIST ? (_dim < 4 ? "hist": "arr") : "sparse") << _dim;
        std::cout << " = " << check << std::endl;
    }

    return check;
}

void hist103_THnSparse_hist() {
    // Create histograms to store results for each test
    TH2D* timeHists[TTimeHists::kNUMHIST][TTimeHists::kNUMTIME]{};
    for (int h{0}; h < TTimeHists::kNUMHIST; h++) {
        for (int t{0}; t < TTimeHists::kNUMTIME; t++) {
            // Set histogram name
            TString name("histTime_");
            
            if ((TTimeHists::EHist)h == TTimeHists::kHIST) {
                name += "arr";
            }
            else {
                name += "sp";
            }

            if (t == 0) name += "_r";

            // Set histogram title
            TString title{};
            title.Form("Throughput (fill,get) %s (%s, 1M entries/sec);dim;bins;1Mentries/sec",
                        (h == 0 ? "TH1/2/3/nD" : "THnSparseD"),
                        (t == 0 ? "real" : "CPU"));
            
            // Make histogram
            timeHists[h][t] = new TH2D(name, title, 6, 0.5, 6.5, 10, 5, 105);
        }
    }

    TH2D* sparseMemHist{new TH2D("sparseMemHist", "Fractional memory usage;dim;bins;fraction of memory used",
                                6, 0.5, 6.5, 10, 5, 105)};
    TH2D* sparseBinsHist{new TH2D("sparseBinHist", "Fractional number of used bins;dim;bins;fraction of filled bins",
                                6, 0.5, 6.5, 10, 5, 105)};


    // Run tests
    double max{-1.};
    for (int dim{1}; dim < 7; dim++) {
        std::cout << "Processing dimension " << dim << "..." << std::endl;
        for (int bins{10}; bins <= 100; bins += 10) {
            std::cout << "Bins = " << bins << "..." << std::endl;
            // Create and run histogram timer
            TTimeHists timer(dim, bins, 1000);
            timer.Run();

            // // Record results
            for (int h{0}; h < TTimeHists::kNUMHIST; h++) {
                for (int t{0}; t < TTimeHists::kNUMTIME; t++) {
                    double time{timer.GetTime((TTimeHists::EHist)h, (TTimeHists::ETime)t)};
                    if (time >= 0.) timeHists[h][t]->Fill(dim, bins, time);
                }
            }

            sparseMemHist->Fill(dim, bins, timer.GetSparse()->GetSparseFractionMem());
            sparseBinsHist->Fill(dim, bins, timer.GetSparse()->GetSparseFractionBins());

            // Update max
            if (max < timer.GetTime(TTimeHists::kSPARSE, TTimeHists::kREAL)) {
                max = timer.GetTime(TTimeHists::kSPARSE, TTimeHists::kREAL);
            }

            fflush(stdout);
        }
    }

    // Style results
    double markerSize{2.5};
    sparseMemHist->SetMarkerSize(markerSize);
    sparseBinsHist->SetMarkerSize(markerSize);

    TH2D* timeHistRatio[TTimeHists::kNUMTIME]{};
    for (int t{0}; t < TTimeHists::kNUMTIME; t++) {
        const char* name{t ? "timeHistRatio" : "timeHistRatio_r"};
        timeHistRatio[t] = (TH2D*)timeHists[TTimeHists::kSPARSE][t]->Clone(name);

        TString title{};
        title.Form("Relative speed improvement (%s, 1M entries/sec):sparse/hist;dim;bin;s#Delta 1M entries/sec",
                    (t == 0 ? "Real" : "CPU"));
        timeHistRatio[t]->SetTitle(title);
        timeHistRatio[t]->Divide(timeHists[TTimeHists::kHIST][t]);
        timeHistRatio[t]->SetMinimum(0.1);
        timeHistRatio[t]->SetMarkerSize(markerSize);
    }

    // Create and style canvas
    TCanvas* c{new TCanvas("c", "c")};
    c->Divide(3, 3);

    gStyle->SetPalette(6, nullptr);
    gStyle->SetPaintTextFormat(".2g");
    gStyle->SetOptStat(0);

    // Draw and write results
    TFile* file{new TFile("sparseHist.root", "RECREATE")};
    const char* opt{"TEXT COL"};
    for (int t{0}; t < TTimeHists::kNUMTIME; t++) {
        for (int h{0}; h < TTimeHists::kNUMHIST; h++) {
            timeHists[h][t]->SetMaximum(max);
            timeHists[h][t]->SetMarkerSize(markerSize);

            c->cd(1 + h + 3 * t);
            timeHists[h][t]->Draw(opt);
            timeHists[h][t]->Write();
        }

        c->cd(3 + t * 3);
        timeHistRatio[t]->Draw(opt);
        gPad->SetLogz();
        timeHistRatio[t]->Write();
    }
    
    c->cd(7);
    sparseMemHist->Draw(opt);
    sparseMemHist->Write();

    c->cd(8);
    sparseBinsHist->Draw(opt);
    sparseBinsHist->Write();

    c->Write();

    delete file;
}

int main() {
    hist103_THnSparse_hist();
    return 0;
}