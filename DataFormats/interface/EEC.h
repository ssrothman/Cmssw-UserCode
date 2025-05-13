#ifndef SROTHMAN_DATAFORMATS_EEC_H
#define SROTHMAN_DATAFORMATS_EEC_H

#include "DataFormats/NanoAOD/interface/FlatTable.h"

namespace EEC{
    template <class ResultType>
    struct CMSSWResult{
        unsigned iJet, iReco;
        ResultType result;

        CMSSWResult() : iJet(0), iReco(0), result() {}

        typedef typename std::conditional<
            ResultType::SHOULD_BIN,
            int,
            float>::type T;

        static constexpr nanoaod::FlatTable::ColumnType COLUMN_TYPE = ResultType::SHOULD_BIN ?
            nanoaod::FlatTable::IntColumn :
            nanoaod::FlatTable::FloatColumn;

        static constexpr bool IS_ARRAY = ResultType::IS_ARRAY;

        template <typename T>
        CMSSWResult(unsigned iJet, unsigned iReco,
                const T& arg) :
            iJet(iJet),
            iReco(iReco),
            result(arg) {}
    };

    template <class TransferResultType>
    struct CMSSWTransferResult{
        unsigned iReco, iGen;
        TransferResultType result;

        typedef typename std::conditional<
            TransferResultType::SHOULD_BIN,
            int,
            float>::type T;

        static constexpr nanoaod::FlatTable::ColumnType COLUMN_TYPE = TransferResultType::SHOULD_BIN ?
            nanoaod::FlatTable::IntColumn :
            nanoaod::FlatTable::FloatColumn;

        static constexpr bool IS_ARRAY = TransferResultType::IS_ARRAY;

        CMSSWTransferResult() : iReco(0), iGen(0), result() {}

        template <typename T>
        CMSSWTransferResult(unsigned iReco, unsigned iGen,
                const T& calc) :
            iReco(iReco),
            iGen(iGen),
            result(calc) {}
    };
};

#endif
