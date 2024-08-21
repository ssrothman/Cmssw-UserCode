#ifndef SROTHMAN_DATAFORMATS_EEC_H
#define SROTHMAN_DATAFORMATS_EEC_H

#include <boost/multi_array.hpp>
#include "SRothman/EECs/src/fastStructs.h"

struct EECresult{
    unsigned iJet, iReco; 

    unsigned maxOrder;
    bool doRes3;
    bool doRes4Shapes;
    bool doRes4Fixed;
    
    std::array<std::shared_ptr<std::vector<double>>, 5> proj;
    std::shared_ptr<boost::multi_array<double, 3>> res3;
    std::shared_ptr<fastEEC::res4shapes<double>> res4shapes;

    EECresult() :
        iJet(9999), iReco(9999),
        maxOrder(0),
        doRes3(false), 
        doRes4Shapes(false), 
        doRes4Fixed(false), 
        proj({{nullptr, nullptr, nullptr, nullptr, nullptr}}),
        res3(nullptr), 
        res4shapes(nullptr) {}

    EECresult(unsigned iJet, unsigned iReco, 
              unsigned maxOrder,
              bool doRes3, 
              bool doRes4Shapes, bool doRes4Fixed,
              std::array<std::shared_ptr<std::vector<double>>, 5> p,
              std::shared_ptr<boost::multi_array<double, 3>> r3,
              std::shared_ptr<fastEEC::res4shapes<double>> r4s) :
        iJet(iJet), iReco(iReco),
        maxOrder(maxOrder),
        doRes3(doRes3),
        doRes4Shapes(doRes4Shapes), 
        doRes4Fixed(doRes4Fixed),
        proj(p),
        res3(r3),
        res4shapes(r4s) {}
};

struct EECtransfer{
    unsigned iReco, iGen;
    unsigned maxOrder;
    bool doRes3;
    bool doRes4Shapes;
    bool doRes4Fixed;

    std::array<std::shared_ptr<boost::multi_array<double, 2>>, 5> proj;
    std::shared_ptr<boost::multi_array<double, 6>> res3;
    std::shared_ptr<fastEEC::res4shapes_transfer<double>> res4shapes;

    EECtransfer() :
        iReco(9999), iGen(9999),
        maxOrder(0),
        doRes3(false),
        doRes4Shapes(false),
        doRes4Fixed(false),
        proj({{nullptr, nullptr, nullptr, nullptr, nullptr}}),
        res3(nullptr),
        res4shapes(nullptr) {}

    EECtransfer(unsigned iReco, unsigned iGen, 
                unsigned maxOrder,
                bool doRes3,
                bool doRes4Shapes, bool doRes4Fixed,
                std::array<std::shared_ptr<boost::multi_array<double, 2>>, 5> p2,
                std::shared_ptr<boost::multi_array<double, 6>> r3,
                std::shared_ptr<fastEEC::res4shapes_transfer<double>> r4s) :
        iReco(iReco), iGen(iGen),
        maxOrder(maxOrder),
        doRes3(doRes3), 
        doRes4Shapes(doRes4Shapes),
        doRes4Fixed(doRes4Fixed), 
        proj(p2),
        res3(r3),
        res4shapes(r4s) {}
};

#endif
