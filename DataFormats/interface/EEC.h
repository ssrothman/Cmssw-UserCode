#ifndef SROTHMAN_DATAFORMATS_EEC_H
#define SROTHMAN_DATAFORMATS_EEC_H

#include "SRothman/EECs/src/Res4Result.h"
#include "SRothman/EECs/src/Res4TransferResult.h"
#include "SRothman/EECs/src/Res4Calculator.h"

namespace EEC{
    struct CMSSWRes4Result{
        unsigned iJet, iReco;
        Res4Result_Vector result;

        CMSSWRes4Result() : iJet(0), iReco(0), result() {}

        CMSSWRes4Result(unsigned iJet, unsigned iReco,
                const Res4Calculator& calc) :
            iJet(iJet), 
            iReco(iReco),
            result(calc) {}

        template <typename T>
        CMSSWRes4Result(unsigned iJet, unsigned iReco,
                const T& arg) :
            iJet(iJet),
            iReco(iReco),
            result(arg) {}
    };

    struct CMSSWRes4TransferResult{
        unsigned iReco, iGen;
        Res4TransferResult_Vector result;

        CMSSWRes4TransferResult() : iReco(0), iGen(0), result() {}

        CMSSWRes4TransferResult(unsigned iReco, unsigned iGen,
                const Res4TransferCalculator& calc) :
            iReco(iReco),
            iGen(iGen),
            result(calc) {}
    };
};

#endif
