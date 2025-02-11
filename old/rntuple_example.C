/**
 * Author: Alaettin Serhan Mete <amete@anl.gov>
 */

#include <iostream>
#include <memory>

#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleReader.hxx>
#include <ROOT/RNTupleWriter.hxx>

#include <TMemFile.h>

using ROOT::Experimental::RNTuple;
using ROOT::Experimental::RNTupleModel;
using ROOT::Experimental::RNTupleReader;
using ROOT::Experimental::RNTupleWriter;

#define ASSERT(condition, message) \
    do { \
        if (!(condition)) {\
            std::cerr << "Assertion failed: " << #condition << " (" << __FILE__ << ":" << __LINE__ << ") " << message << std::endl; \
            std::abort(); \
        } \
    } while (false)

void rntuple_example() {
    // Write an in-memory file
    std::unique_ptr<TMemFile> file{std::make_unique<TMemFile>("file.root", "RECREATE")};

    {
        // Create a bare model
        std::unique_ptr<RNTupleModel> model{RNTupleModel::Create()};

        // Now add a new top-level field that's going to hold a vector of integers
        std::shared_ptr<ROOT::Internal::RUidColl> field{model->MakeField<std::vector<int>>("foo", 0)};
        
        // Create the RNTupleWriter from the model
        // At this point, the model is null
        std::unique_ptr<RNTupleWriter> ntuple{RNTupleWriter::Append(std::move(model), "ntuple", *file)};

        // Write 10 rows into the ntuple
        for (size_t i{0}; i < 10; i++) {
            // For each row, clear the vector
            field->clear();
            
            // For each row, write i values into the vector
            for (size_t j{0}; j < i + 1; j++) {
                field->push_back(i * j);
            }

            // Now fill the ntuple
            ntuple->Fill();
        }
    }   // End of scope, where RNTuple is committed

    // Read the file using the anchor from the in-memory file
    std::unique_ptr<RNTupleReader> ntuple{RNTupleReader::Open(file->Get<RNTuple>("ntuple"))};

    // Get the view object for the field we want to read
    ROOT::Experimental::RNTupleView<ROOT::Internal::RUidColl, false> view{ntuple->GetView<std::vector<int>>("foo")};

    // Read the third row
    std::vector<int> result{view(2)};

    // Check to ensure we have the correct data
    ASSERT(result.size() == 3, "Vector size should be 3");
    ASSERT(result.at(0) == 0, "First element should be 0");
    ASSERT(result.at(1) == 2, "Second element should be 2");
    ASSERT(result.at(2) == 4, "Third element should be 4");
}