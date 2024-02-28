#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "SRothman/DataFormats/interface/jets.h"
#include "SRothman/DataFormats/interface/matching.h"
#include "SRothman/DataFormats/interface/EEC.h"

#include "SRothman/SimonTools/src/deltaR.h"
#include "SRothman/SimonTools/src/util.h"

#include "SRothman/EECs/src/eec_oo.h"
#include "SRothman/EECs/src/fast.h"
#include "SRothman/EECs/src/fastStructs.h"

#include <iostream>
#include <memory>
#include <vector>

#include <boost/histogram.hpp>
#include <boost/multi_array.hpp>

static arma::mat matFromArray(const boost::multi_array<double, 2>& arr){
    arma::mat mat(arr.shape()[1], arr.shape()[0]);
    for(unsigned i=0; i<arr.shape()[0]; ++i){
        for(unsigned j=0; j<arr.shape()[1]; ++j){
            mat(j, i) = arr[i][j];//NB we transpose
        }
    }
    return mat;
}

class EECProducer : public edm::stream::EDProducer<> {
public:
    explicit EECProducer(const edm::ParameterSet&);
    ~EECProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    int verbose_;

    unsigned maxOrder_;
    bool doRes3_, doRes4_;
    std::string ptNorm_;

    edm::InputTag recoTag_;
    edm::EDGetTokenT<edm::View<jet>> recoToken_;

    bool doGen_;
    edm::InputTag genTag_;
    edm::EDGetTokenT<edm::View<jet>> genToken_;
    edm::InputTag matchTag_;
    edm::EDGetTokenT<edm::View<jetmatch>> matchToken_;

    std::vector<double> dRbinEdges_;

    std::vector<double> dRbinEdges_coarse_;

    std::vector<double> xibinEdges_;
    std::vector<double> phibinEdges_;

    std::vector<double> r_dipole_edges_;
    std::vector<double> ct_dipole_edges_;
    
    std::vector<double> r_tee_edges_;
    std::vector<double> ct_tee_edges_;

    std::vector<double> r_triangle_edges_;
    std::vector<double> ct_triangle_edges_;

    double shapetol_;

    void addProjected(EECresult& next, const EECCalculator& calc, 
            bool PU);
    void addResolved(EECresult& next, const EECCalculator& calc, 
            bool PU);
    void addTransfer(EECtransfer& next, const EECCalculator& calc);
};


void EECProducer::addTransfer(EECtransfer& next, 
                              const EECCalculator& calc){

    for(unsigned order=2; order<=calc.getMaxOrder(); ++order){
        next.orders.emplace_back(order);
        next.proj.emplace_back(calc.getTransferproj(order));
    }

    if(doRes3_){
        next.res3 = calc.getTransferres3();
    }
    if(doRes4_){
        next.res4 = calc.getTransferres4();
    }
}

void EECProducer::addProjected(EECresult& next, 
                                const EECCalculator& calc,
                                bool PU){
    for(unsigned order=2; order<=calc.getMaxOrder(); ++order){
        next.orders.emplace_back(order);
        if(PU){
            next.wts.emplace_back(calc.getproj_PU(order));
        } else {
            next.wts.emplace_back(calc.getproj(order));
        }
    }
}

void EECProducer::addResolved(EECresult& next, 
                                const EECCalculator& calc,
                                bool PU){
    if(doRes3_){
        if(PU){
            next.res3wts = calc.getres3_PU();
        } else {
            next.res3wts = calc.getres3();
        }
    }
    if(doRes4_){
        if(PU){
            next.res4wts = calc.getres4_PU();
        } else {
            next.res4wts = calc.getres4();
        }
    }
}

EECProducer::EECProducer(const edm::ParameterSet& conf)
        : verbose_(conf.getParameter<int>("verbose")),
          maxOrder_(conf.getParameter<unsigned>("maxOrder")),
          doRes3_(conf.getParameter<bool>("doRes3")),
          doRes4_(conf.getParameter<bool>("doRes4")),
          ptNorm_(conf.getParameter<std::string>("ptNorm")),
          recoTag_(conf.getParameter<edm::InputTag>("reco")),
          recoToken_(consumes<edm::View<jet>>(recoTag_)),
          doGen_(conf.getParameter<bool>("doGen")),
          genTag_(conf.getParameter<edm::InputTag>("gen")),
          matchTag_(conf.getParameter<edm::InputTag>("match")),
          dRbinEdges_(conf.getParameter<std::vector<double>>("dRbinEdges")),

          dRbinEdges_coarse_(conf.getParameter<std::vector<double>>("dRbinEdges_coarse")),

          xibinEdges_(conf.getParameter<std::vector<double>>("xibinEdges")),
          phibinEdges_(conf.getParameter<std::vector<double>>("phibinEdges")),

          r_dipole_edges_(conf.getParameter<std::vector<double>>("r_dipole_edges")),
          ct_dipole_edges_(conf.getParameter<std::vector<double>>("ct_dipole_edges")),

          r_tee_edges_(conf.getParameter<std::vector<double>>("r_tee_edges")),
          ct_tee_edges_(conf.getParameter<std::vector<double>>("ct_tee_edges")),

          r_triangle_edges_(conf.getParameter<std::vector<double>>("r_triangle_edges")),
          ct_triangle_edges_(conf.getParameter<std::vector<double>>("ct_triangle_edges")),

          shapetol_(conf.getParameter<double>("shapetol")) {
    produces<std::vector<EECresult>>("reco");
    produces<std::vector<EECresult>>("recoPU");
    if(doGen_){
        produces<std::vector<EECresult>>("gen");
        produces<std::vector<EECresult>>("genUNMATCH");
        produces<std::vector<EECtransfer>>("transfer");
        genToken_ = consumes<edm::View<jet>>(genTag_);
        matchToken_ = consumes<edm::View<jetmatch>>(matchTag_);
    }
}

void EECProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("reco");
  desc.add<edm::InputTag>("gen");
  desc.add<edm::InputTag>("match");

  desc.add<int>("verbose");

  desc.add<unsigned>("maxOrder");
  desc.add<bool>("doRes3");
  desc.add<bool>("doRes4");
  desc.add<std::string>("ptNorm");

  desc.add<bool>("doGen");

  desc.add<std::vector<double>>("dRbinEdges");
  desc.add<std::vector<double>>("dRbinEdges_coarse");

  desc.add<std::vector<double>>("xibinEdges");
  desc.add<std::vector<double>>("phibinEdges");

  desc.add<std::vector<double>>("r_dipole_edges");
  desc.add<std::vector<double>>("ct_dipole_edges");

  desc.add<std::vector<double>>("r_tee_edges");
  desc.add<std::vector<double>>("ct_tee_edges");

  desc.add<std::vector<double>>("r_triangle_edges");
  desc.add<std::vector<double>>("ct_triangle_edges");

  desc.add<double>("shapetol");

  descriptions.addWithDefaultLabel(desc);
}

void EECProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  if(verbose_){
    printf("top of EECProducer::produce\n");
    fflush(stdout);
  }

  edm::Handle<edm::View<jet>> reco;
  evt.getByToken(recoToken_, reco);

  edm::Handle<edm::View<jet>> gen;
  edm::Handle<edm::View<jetmatch>> matches;
  if(doGen_){
      evt.getByToken(genToken_, gen);
      evt.getByToken(matchToken_, matches);
  }
  if(verbose_){
    printf("got from event\n");
    fflush(stdout);
  }

  auto result = std::make_unique<std::vector<EECresult>>();
  auto resultPU = std::make_unique<std::vector<EECresult>>();

  auto resultgen = std::make_unique<std::vector<EECresult>>();
  auto resultUNMATCH = std::make_unique<std::vector<EECresult>>();
  auto resulttrans = std::make_unique<std::vector<EECtransfer>>();

  auto RLax = std::make_shared<boost::histogram::axis::variable<double>>(dRbinEdges_);
  auto RLax_coarse = std::make_shared<boost::histogram::axis::variable<double>>(dRbinEdges_coarse_);

  auto xiax = std::make_shared<boost::histogram::axis::variable<double>>(xibinEdges_);
  auto phiax = std::make_shared<boost::histogram::axis::variable<double>>(phibinEdges_);

  auto r_dipole_ax = std::make_shared<boost::histogram::axis::variable<double>>(r_dipole_edges_);
  auto ct_dipole_ax = std::make_shared<boost::histogram::axis::variable<double>>(ct_dipole_edges_);

  auto r_tee_ax = std::make_shared<boost::histogram::axis::variable<double>>(r_tee_edges_);
  auto ct_tee_ax = std::make_shared<boost::histogram::axis::variable<double>>(ct_tee_edges_);

  auto r_triangle_ax = std::make_shared<boost::histogram::axis::variable<double>>(r_triangle_edges_);
  auto ct_triangle_ax = std::make_shared<boost::histogram::axis::variable<double>>(ct_triangle_edges_);

  fastEEC::normType norm;
  if(ptNorm_ == "RAW"){
      norm = fastEEC::normType::RAWPT;
  }else if(ptNorm_ == "CORR"){
      norm = fastEEC::normType::CORRPT;
  }else if(ptNorm_ == "SUM"){
      norm = fastEEC::normType::SUMPT;
  }else{
      throw cms::Exception("Bad norm type");
  }

  for(unsigned iReco=0; iReco<reco->size(); ++iReco){
      if(verbose_){
        printf("iReco %u\n", iReco);
        fflush(stdout);
      }

      if(reco->at(iReco).nPart < 2){
          printf("Reco jet with <2 constituents, skipping\n");
          printf("Don't think this should actually ever happen\n");
          throw cms::Exception("Bad reco jet");
          continue;
      }

      int iGen=-1;
      arma::mat ptrans;
      std::vector<bool> PU, UNMATCHED;

      PU.resize(reco->at(iReco).nPart, true);

      if(doGen_){
          int matchidx=-1;
          for(unsigned iM=0; iM<matches->size(); ++iM){
              if(matches->at(iM).iReco == iReco){
                matchidx = iM;
                break;
              }
          }
          if(matchidx>=0){
              iGen = matches->at(matchidx).iGen;
              UNMATCHED.resize(gen->at(iGen).nPart, true);
              ptrans = matches->at(matchidx).ptrans;

              for(unsigned iPReco=0; iPReco<reco->at(iReco).nPart; ++iPReco){
                  for(unsigned iPGen=0; iPGen<gen->at(iGen).nPart; ++iPGen){
                      if(ptrans(iPReco, iPGen) > 0.){
                          PU.at(iPReco) = false;
                          UNMATCHED.at(iPGen) = false;
                      }
                  }
              }
          }
      }

      if(verbose_){
        printf("iGen %d\n", iGen);
      }

      auto startreco = std::chrono::high_resolution_clock::now();
      auto ans_reco = fastEEC::fastEEC<double, true, false>(
              reco->at(iReco), RLax, maxOrder_, norm,
              RLax_coarse, xiax, phiax, 
              r_dipole_ax, ct_dipole_ax,
              r_tee_ax, ct_tee_ax,
              r_triangle_ax, ct_triangle_ax,
              shapetol_,
              &PU
      );
      auto endreco = std::chrono::high_resolution_clock::now();
      printf("fast reco: %f\n", std::chrono::duration_cast<std::chrono::microseconds>(endreco - startreco).count() / 1000000.);
        
      printf("RECO\n");
      printf("\tsumwt2: %f\n", std::accumulate(ans_reco.wts2.begin(), ans_reco.wts2.end(), 0.));
      printf("\tsumwt3: %f\n", std::accumulate(ans_reco.wts3.begin(), ans_reco.wts3.end(), 0.));
      printf("\tsumwt4: %f\n", std::accumulate(ans_reco.wts4.begin(), ans_reco.wts4.end(), 0.));
      printf("\tsumwt5: %f\n", std::accumulate(ans_reco.wts5.begin(), ans_reco.wts5.end(), 0.));
      printf("\tsumwt6: %f\n", std::accumulate(ans_reco.wts6.begin(), ans_reco.wts6.end(), 0.));
      printf("\n");
      printf("\tsumwt2_PU: %f\n", std::accumulate(ans_reco.wts2_PU.begin(), ans_reco.wts2_PU.end(), 0.));
      printf("\tsumwt3_PU: %f\n", std::accumulate(ans_reco.wts3_PU.begin(), ans_reco.wts3_PU.end(), 0.));
      printf("\tsumwt4_PU: %f\n", std::accumulate(ans_reco.wts4_PU.begin(), ans_reco.wts4_PU.end(), 0.));
      printf("\tsumwt5_PU: %f\n", std::accumulate(ans_reco.wts5_PU.begin(), ans_reco.wts5_PU.end(), 0.));
      printf("\tsumwt6_PU: %f\n", std::accumulate(ans_reco.wts6_PU.begin(), ans_reco.wts6_PU.end(), 0.));
      printf("\n");
      double sumres3=0, sumres3PU=0;
      for(unsigned i=0; i<ans_reco.resolved3.shape()[0]; ++i){
          for(unsigned j=0; j<ans_reco.resolved3.shape()[1]; ++j){
              for(unsigned k=0; k<ans_reco.resolved3.shape()[2]; ++k){
                  sumres3 += ans_reco.resolved3[i][j][k];
                  sumres3PU += ans_reco.resolved3_PU[i][j][k];
              }
          }
      }
      printf("\tsumres3: %f\n", sumres3);
      printf("\tsumres3PU: %f\n", sumres3PU);
      printf("\n");

      double sumshape0=0, sumshape1=0, sumshape2=0, sumshape3=0;
      double sumtotPU=0;
      for(unsigned i=0; i<ans_reco.resolved4_shapes.shape()[1]; ++i){
          for(unsigned j=0; j<ans_reco.resolved4_shapes.shape()[2]; ++j){
              for(unsigned k=0; k<ans_reco.resolved4_shapes.shape()[3]; ++k){
                  sumshape0 += ans_reco.resolved4_shapes[0][i][j][k];
                  sumshape1 += ans_reco.resolved4_shapes[1][i][j][k];
                  sumshape2 += ans_reco.resolved4_shapes[2][i][j][k];
                  sumshape3 += ans_reco.resolved4_shapes[3][i][j][k];

                  sumtotPU += ans_reco.resolved4_shapes_PU[0][i][j][k];
                  sumtotPU += ans_reco.resolved4_shapes_PU[1][i][j][k];
                  sumtotPU += ans_reco.resolved4_shapes_PU[2][i][j][k];
                  sumtotPU += ans_reco.resolved4_shapes_PU[3][i][j][k];
              }
          }
      }
      printf("\tsumshape0: %f\n", sumshape0);
      printf("\tsumshape1: %f\n", sumshape1);
      printf("\tsumshape2: %f\n", sumshape2);
      printf("\tsumshape3: %f\n", sumshape3);
      printf("\n");
      printf("\tsumtot: %f\n", sumshape0+sumshape1+sumshape2+sumshape3);
      printf("\tsumtotPU: %f\n", sumtotPU);
      printf("\n");

      /*double sumfixed0=0, sumfixed1=0, sumfixed2=0;
      double sumfixedPU=0;
      for(unsigned i=0; i<ans_reco.resolved4_fixed.shape()[1]; ++i){
        sumfixed0 += ans_reco.resolved4_fixed[0][i];
        sumfixed1 += ans_reco.resolved4_fixed[1][i];
        sumfixed2 += ans_reco.resolved4_fixed[2][i];

        sumfixedPU += ans_reco.resolved4_fixed_PU[0][i];
        sumfixedPU += ans_reco.resolved4_fixed_PU[1][i];
        sumfixedPU += ans_reco.resolved4_fixed_PU[2][i];
      }
      printf("sumfixedshape0: %f\n", sumfixed0);
      printf("sumfixedshape1: %f\n", sumfixed1);
      printf("sumfixedshape2: %f\n", sumfixed2);
      printf("sumfixedPU: %f\n", sumfixedPU);
      printf("\n");*/

      if(verbose_){
        printf("ran calc\n");
      }

      EECresult next;
      next.iJet = iReco;
      next.iReco = iReco;
      EECresult nextPU;
      nextPU.iJet = iReco;
      nextPU.iReco = iReco;
      if(maxOrder_ >= 2){
          next.orders.push_back(2);
          nextPU.orders.push_back(2);
          next.wts.push_back(ans_reco.wts2);
          nextPU.wts.push_back(ans_reco.wts2_PU);
      }
      if(maxOrder_ >= 3){
          next.orders.push_back(3);
          nextPU.orders.push_back(3);
          next.wts.push_back(ans_reco.wts3);
          nextPU.wts.push_back(ans_reco.wts3_PU);
      }
      if(maxOrder_ >= 4){
          next.orders.push_back(4);
          nextPU.orders.push_back(4);
          next.wts.push_back(ans_reco.wts4);
          nextPU.wts.push_back(ans_reco.wts4_PU);
      }
      if(maxOrder_ >= 5){
          next.orders.push_back(5);
          nextPU.orders.push_back(5);
          next.wts.push_back(ans_reco.wts5);
          nextPU.wts.push_back(ans_reco.wts5_PU);
      }
      if(maxOrder_ >= 6){
          next.orders.push_back(6);
          nextPU.orders.push_back(6);
          next.wts.push_back(ans_reco.wts6);
          nextPU.wts.push_back(ans_reco.wts6_PU);
      }

      result->push_back(std::move(next));
      if(verbose_){
        printf("pushed back result\n");
      }

      resultPU->push_back(std::move(nextPU));
      if(verbose_){
        printf("pushed back resultPU\n");
      }

      if(iGen >=0 ){
          auto startgen = std::chrono::high_resolution_clock::now();
          auto ans_gen = fastEEC::fastEEC<double, true, true>(
                    gen->at(iGen), RLax, maxOrder_, norm, 
                    RLax_coarse, xiax, phiax,
                    r_dipole_ax, ct_dipole_ax,
                    r_tee_ax, ct_tee_ax,
                    r_triangle_ax, ct_triangle_ax,
                    shapetol_,
                    &UNMATCHED, &(reco->at(iReco)), &ptrans
          );
          auto endgen = std::chrono::high_resolution_clock::now();

          printf("fast gen: %f\n", std::chrono::duration_cast<std::chrono::microseconds>(endgen - startgen).count() / 1000000.);
          printf("\n");

          printf("\tsumwt2: %f\n", std::accumulate(ans_gen.wts2.begin(), ans_gen.wts2.end(), 0.));
          printf("\tsumwt3: %f\n", std::accumulate(ans_gen.wts3.begin(), ans_gen.wts3.end(), 0.));
          printf("\tsumwt4: %f\n", std::accumulate(ans_gen.wts4.begin(), ans_gen.wts4.end(), 0.));
          printf("\tsumwt5: %f\n", std::accumulate(ans_gen.wts5.begin(), ans_gen.wts5.end(), 0.));
          printf("\tsumwt6: %f\n", std::accumulate(ans_gen.wts6.begin(), ans_gen.wts6.end(), 0.));
          printf("\n");
          printf("\tsumwt2_PU: %f\n", std::accumulate(ans_gen.wts2_PU.begin(), ans_gen.wts2_PU.end(), 0.));
          printf("\tsumwt3_PU: %f\n", std::accumulate(ans_gen.wts3_PU.begin(), ans_gen.wts3_PU.end(), 0.));
          printf("\tsumwt4_PU: %f\n", std::accumulate(ans_gen.wts4_PU.begin(), ans_gen.wts4_PU.end(), 0.));
          printf("\tsumwt5_PU: %f\n", std::accumulate(ans_gen.wts5_PU.begin(), ans_gen.wts5_PU.end(), 0.));
          printf("\tsumwt6_PU: %f\n", std::accumulate(ans_gen.wts6_PU.begin(), ans_gen.wts6_PU.end(), 0.));
          printf("\n");

          double sumtrans2 = 0, sumtrans3 = 0, sumtrans4 = 0, sumtrans5 = 0, sumtrans6 = 0;
          for(unsigned iReco=0; iReco<ans_gen.wts2.size(); ++iReco){
              for(unsigned iGen=0; iGen<ans_gen.wts2.size(); ++iGen){
                  sumtrans2 += ans_gen.transfer2[iGen][iReco];
                  sumtrans3 += ans_gen.transfer3[iGen][iReco];
                  sumtrans4 += ans_gen.transfer4[iGen][iReco];
                  //sumtrans5 += ans_gen.transfer5[iGen][iReco];
                  //sumtrans6 += ans_gen.transfer6[iGen][iReco];
              }
          }


          printf("\t1-sumtrans2: %f\n", 1-sumtrans2);
          printf("\t1-sumtrans3: %f\n", 1-sumtrans3);
          printf("\t1-sumtrans4: %f\n", 1-sumtrans4);
          //printf("\t1-sumtrans5: %f\n", 1-sumtrans5);
          //printf("\t1-sumtrans6: %f\n", 1-sumtrans6);
          printf("\n");

          double sumres3_gen=0, sumres3PU_gen=0;
          double sumres3_trans=0;
          for(unsigned i=0; i<ans_reco.resolved3.shape()[0]; ++i){
              for(unsigned j=0; j<ans_reco.resolved3.shape()[1]; ++j){
                  for(unsigned k=0; k<ans_reco.resolved3.shape()[2]; ++k){
                      sumres3_gen += ans_gen.resolved3[i][j][k];
                      sumres3PU_gen += ans_gen.resolved3_PU[i][j][k];
                      for(unsigned a=0; a<ans_gen.resolved3.shape()[0]; ++a){
                          for(unsigned b=0; b<ans_gen.resolved3.shape()[1]; ++b){
                              for(unsigned c=0; c<ans_gen.resolved3.shape()[2]; ++c){
                                  sumres3_trans += ans_gen.transfer_res3[i][j][k][a][b][c];
                              }
                          }
                      }
                  }
              }
          }
          printf("\tsumres3: %f\n", sumres3_gen);
          printf("\tsumres3PU: %f\n", sumres3PU_gen);
          printf("\tsumres3_trans: %f\n", sumres3_trans);
          printf("\n");

          double sumshape0_gen=0, sumshape1_gen=0, sumshape2_gen=0, sumshape3_gen=0;
          double sumtotPU_gen=0;
          double sumtrans_res4=0;
          for(unsigned i=0; i<ans_reco.resolved4_shapes.shape()[1]; ++i){
              for(unsigned j=0; j<ans_reco.resolved4_shapes.shape()[2]; ++j){
                  for(unsigned k=0; k<ans_reco.resolved4_shapes.shape()[3]; ++k){
                      sumshape0_gen += ans_gen.resolved4_shapes[0][i][j][k];
                      sumshape1_gen += ans_gen.resolved4_shapes[1][i][j][k];
                      sumshape2_gen += ans_gen.resolved4_shapes[2][i][j][k];
                      sumshape3_gen += ans_gen.resolved4_shapes[3][i][j][k];

                      sumtotPU_gen += ans_gen.resolved4_shapes_PU[0][i][j][k];
                      sumtotPU_gen += ans_gen.resolved4_shapes_PU[1][i][j][k];
                      sumtotPU_gen += ans_gen.resolved4_shapes_PU[2][i][j][k];
                      sumtotPU_gen += ans_gen.resolved4_shapes_PU[3][i][j][k];
                      for(unsigned a=0; a<4; ++a){
                          for(unsigned b=0; b<ans_gen.resolved4_shapes.shape()[1]; ++b){
                              for(unsigned c=0; c<ans_gen.resolved4_shapes.shape()[2]; ++c){
                                  for(unsigned d=0; d<ans_gen.resolved4_shapes.shape()[3]; ++d){
                                      sumtrans_res4 += ans_gen.transfer_res4_shapes[0][i][j][k][a][b][c][d];
                                      sumtrans_res4 += ans_gen.transfer_res4_shapes[1][i][j][k][a][b][c][d];
                                      sumtrans_res4 += ans_gen.transfer_res4_shapes[2][i][j][k][a][b][c][d];
                                      sumtrans_res4 += ans_gen.transfer_res4_shapes[3][i][j][k][a][b][c][d];
                                  }
                              }
                          }
                      }
                  }
              }
          }
          printf("\tsumshape0: %f\n", sumshape0_gen);
          printf("\tsumshape1: %f\n", sumshape1_gen);
          printf("\tsumshape2: %f\n", sumshape2_gen);
          printf("\tsumshape3: %f\n", sumshape3_gen);
          printf("\n");
          printf("\tsumtot: %f\n", sumshape0_gen+sumshape1_gen+sumshape2_gen+sumshape3_gen);
          printf("\tsumtotPU: %f\n", sumtotPU_gen);
          printf("\tsumtrans_res4: %f\n", sumtrans_res4);
          printf("\n");

          /*
          double sumfixed0_gen=0, sumfixed1_gen=0, sumfixed2_gen=0;
          double sumfixedPU_gen=0;
          double sumtransferfixed=0;
          for(unsigned i=0; i<ans_reco.resolved4_fixed.shape()[1]; ++i){
            sumfixed0_gen += ans_gen.resolved4_fixed[0][i];
            sumfixed1_gen += ans_gen.resolved4_fixed[1][i];
            sumfixed2_gen += ans_gen.resolved4_fixed[2][i];

            sumfixedPU_gen += ans_gen.resolved4_fixed_PU[0][i];
            sumfixedPU_gen += ans_gen.resolved4_fixed_PU[1][i];
            sumfixedPU_gen += ans_gen.resolved4_fixed_PU[2][i];
            for(unsigned a=0; a<3; ++a){
                for(unsigned b=0; b<ans_gen.resolved4_fixed.shape()[1]; ++b){
                    sumtransferfixed += ans_gen.transfer_res4_fixed[0][i][a][b];
                    sumtransferfixed += ans_gen.transfer_res4_fixed[1][i][a][b];
                    sumtransferfixed += ans_gen.transfer_res4_fixed[2][i][a][b];
                }
            }
          }
          printf("sumfixedshape0: %f\n", sumfixed0);
          printf("sumfixedshape1: %f\n", sumfixed1);
          printf("sumfixedshape2: %f\n", sumfixed2);
          printf("sumfixedPU: %f\n", sumfixedPU);
          printf("sumtransferfixed: %f\n", sumtransferfixed);
          printf("\n");*/

        if(verbose_){
            printf("ran gen calc with %u (gen) x %u (reco) particles\n", gen->at(iGen).nPart, reco->at(iReco).nPart);
        }

        EECresult nextGen;
        nextGen.iJet = iGen;
        nextGen.iReco = iReco;

        EECresult nextGenUNMATCH;
        nextGenUNMATCH.iReco = iReco;
        nextGenUNMATCH.iJet = iGen;

        EECtransfer nextTransfer;
        nextTransfer.iReco = iReco;
        nextTransfer.iGen = iGen;
        
        if(maxOrder_ >= 2){
          nextGen.orders.push_back(2);
          nextGenUNMATCH.orders.push_back(2);
          nextTransfer.orders.push_back(2);
          nextGen.wts.push_back(ans_gen.wts2);
          nextGenUNMATCH.wts.push_back(ans_gen.wts2_PU);
          nextTransfer.proj.push_back(matFromArray(ans_gen.transfer2));
        }
        if(maxOrder_ >= 3){
          nextGen.orders.push_back(3);
          nextGenUNMATCH.orders.push_back(3);
          nextTransfer.orders.push_back(3);
          nextGen.wts.push_back(ans_gen.wts3);
          nextGenUNMATCH.wts.push_back(ans_gen.wts3_PU);
          nextTransfer.proj.push_back(matFromArray(ans_gen.transfer3));
        }
        if(maxOrder_ >= 4){
          nextGen.orders.push_back(4);
          nextGenUNMATCH.orders.push_back(4);
          nextTransfer.orders.push_back(4);
          nextGen.wts.push_back(ans_gen.wts4);
          nextGenUNMATCH.wts.push_back(ans_gen.wts4_PU);
          nextTransfer.proj.push_back(matFromArray(ans_gen.transfer4));
        }
        if(maxOrder_ >= 5){
          nextGen.orders.push_back(5);
          nextGenUNMATCH.orders.push_back(5);
          nextTransfer.orders.push_back(5);
          nextGen.wts.push_back(ans_gen.wts5);
          nextGenUNMATCH.wts.push_back(ans_gen.wts5_PU);
          nextTransfer.proj.push_back(matFromArray(ans_gen.transfer5));
        }
        if(maxOrder_ >= 6){
          nextGen.orders.push_back(6);
          nextGenUNMATCH.orders.push_back(6);
          nextTransfer.orders.push_back(6);
          nextGen.wts.push_back(ans_gen.wts6);
          nextGenUNMATCH.wts.push_back(ans_gen.wts6_PU);
          nextTransfer.proj.push_back(matFromArray(ans_gen.transfer6));
        }


        resultgen->push_back(std::move(nextGen));
        if(verbose_){
          printf("pushed back gen result\n");
        }

        resultUNMATCH->push_back(std::move(nextGenUNMATCH));
        if(verbose_){
          printf("pushed back genUNMATCH result\n");
        }

        resulttrans->push_back(std::move(nextTransfer));
        if(verbose_){
          printf("pushed back transfer matrices\n");
        }
      }
  }

  evt.put(std::move(result), "reco");
  evt.put(std::move(resultPU), "recoPU");
  if(doGen_){
      evt.put(std::move(resultgen), "gen");
      evt.put(std::move(resulttrans), "transfer");
      evt.put(std::move(resultUNMATCH), "genUNMATCH");
  }
  if(verbose_){
      printf("put into event\n");
  }
}  // end produce()

DEFINE_FWK_MODULE(EECProducer);
