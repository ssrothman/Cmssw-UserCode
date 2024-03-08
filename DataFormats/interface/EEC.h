#ifndef SROTHMAN_DATAFORMATS_EEC_H
#define SROTHMAN_DATAFORMATS_EEC_H

#include <boost/multi_array.hpp>

struct EECresult{
    using vecptr = std::shared_ptr<std::vector<double>>;
    using array2ptr = std::shared_ptr<boost::multi_array<double, 2>>;
    using array3ptr = std::shared_ptr<boost::multi_array<double, 3>>;
    using array4ptr = std::shared_ptr<boost::multi_array<double, 4>>;

    unsigned iJet, iReco; 

    std::vector<vecptr> proj; //orders 2, 3, 4, 5, 6 
    unsigned maxOrder;

    bool doRes3;
    array3ptr res3;

    bool doRes4Shapes;
    array4ptr res4shapes;

    bool doRes4Fixed;
    array2ptr res4fixed;
    
    EECresult() :
        iJet(9999), iReco(9999),
        proj({nullptr, nullptr, nullptr, nullptr, nullptr}),
        maxOrder(0),
        doRes3(false), res3(nullptr),
        doRes4Shapes(false), res4shapes(nullptr),
        doRes4Fixed(false), res4fixed(nullptr) {}

    EECresult(unsigned iJet, unsigned iReco, 
              unsigned maxOrder,
              bool doRes3, 
              bool doRes4Shapes, bool doRes4Fixed,
              vecptr p2, vecptr p3, vecptr p4, vecptr p5, vecptr p6,
              array3ptr r3, array4ptr r4s, array2ptr r4f) :
        iJet(iJet), iReco(iReco),
        proj({p2, p3, p4, p5, p6}),
        maxOrder(maxOrder),
        doRes3(doRes3), res3(r3),
        doRes4Shapes(doRes4Shapes), res4shapes(r4s),
        doRes4Fixed(doRes4Fixed), res4fixed(r4f) {}

};

struct EECtransfer{
    using array2ptr = std::shared_ptr<boost::multi_array<double, 2>>;
    using array4ptr = std::shared_ptr<boost::multi_array<double, 4>>;
    using array6ptr = std::shared_ptr<boost::multi_array<double, 6>>;
    using array8ptr = std::shared_ptr<boost::multi_array<double, 8>>;

    unsigned iReco, iGen;
    unsigned maxOrder;
    std::vector<array2ptr> proj;
    bool doRes3;
    array6ptr res3;
    bool doRes4Shapes;
    array8ptr res4shapes;
    bool doRes4Fixed;
    array4ptr res4fixed;

    EECtransfer(unsigned iReco, unsigned iGen, 
                unsigned maxOrder,
                bool doRes3,
                bool doRes4Shapes, bool doRes4Fixed,
                array2ptr p2, array2ptr p3, array2ptr p4, 
                array2ptr p5, array2ptr p6,
                array6ptr r3, array8ptr r4s, array4ptr r4f) :
        iReco(iReco), iGen(iGen),
        maxOrder(maxOrder),
        proj({p2, p3, p4, p5, p6}),
        doRes3(doRes3), res3(r3),
        doRes4Shapes(doRes4Shapes), res4shapes(r4s),
        doRes4Fixed(doRes4Fixed), res4fixed(r4f) {}

    EECtransfer() :
        iReco(9999), iGen(9999),
        maxOrder(0),
        proj({nullptr, nullptr, nullptr, nullptr, nullptr}),
        doRes3(false), res3(nullptr),
        doRes4Shapes(false), res4shapes(nullptr),
        doRes4Fixed(false), res4fixed(nullptr) {}

};

#endif
