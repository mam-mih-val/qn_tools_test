#include <DataContainer.hpp>
#include <StatBin.hpp>
#include <TFile.h>

int main(int n_args, char **args) {
  assert(n_args==2);
  std::string file_name=args[1];
  auto file =
      TFile::Open(file_name.data());
  assert(file);

  // u1Q1 correlation
  // Axes: {"Centrality", 8, 0, 40}, {"mdc_vtx_tracks_pT", 16, 0, 1.6}, {"y_cm", 15, -0.75, 0.75}
  Qn::DataContainerStats *container_un_Qa;
  // Correlations needed for resolution calculation
  // All are single-axis {"Centrality", 8, 0, 40}
  Qn::DataContainerStats *container_Qa_Qb;
  Qn::DataContainerStats *container_Qb_Qc;
  Qn::DataContainerStats *container_Qa_Qc;

  // reading containers from file
  file->GetObject("u_RESCALED_W1_RECENTERED_Q1x_Q1x",
                  container_un_Qa); // Observable, 3-axes: {"Centrality", 8, 0, 40}, {"mdc_vtx_tracks_pT", 16, 0, 1.6}, {"y_cm", 15, -0.75, 0.75}
  file->GetObject("W1_RECENTERED_W2_RECENTERED_Q1x_Q1x",
      container_Qa_Qb); // Reference, single-axis {"Centrality", 8, 0, 40}
  file->GetObject("W1_RECENTERED_W3_RECENTERED_Q1x_Q1x",
      container_Qa_Qc); // Reference, single-axis {"Centrality", 8, 0, 40}
  file->GetObject("W2_RECENTERED_W3_RECENTERED_Q1x_Q1x",
      container_Qb_Qc); // Reference, single-axis {"Centrality", 8, 0, 40}

  // Copying stats containers to stat bins
  Qn::DataContainer<Qn::StatBin> stat_bins_un_Qa( *container_un_Qa );
  Qn::DataContainer<Qn::StatBin> stat_bins_Qa_Qb( *container_Qa_Qb );
  Qn::DataContainer<Qn::StatBin> stat_bins_Qa_Qc( *container_Qa_Qc );
  Qn::DataContainer<Qn::StatBin> stat_bins_Qb_Qc( *container_Qb_Qc );

  // Resolution calculation with 3-Sub method
  auto resolution = Sqrt(stat_bins_Qa_Qb * stat_bins_Qa_Qc / stat_bins_Qb_Qc);
  // Flow calculation
  auto flow = stat_bins_un_Qa / resolution;

  // Checking rebinnig of resolution (1D, Centrality dependence only)
  Qn::DataContainer<Qn::StatBin> resolution_rebinned;
  try{
    std::cout << "Trying to rebin resolution container centrality axis, halve bins in same range" << std::endl;
    resolution_rebinned = resolution.Rebin({"Centrality", 4, 0.0, 40.0});
    std::cout << "Rebin attempt was successful for resolution container" << std::endl;
    std::cout << "*************************" << std::endl;
    std::cout << "initial resoltuion container" << std::endl;
    int i=0;
    for( const auto &bin : resolution  ){
      std::cout << i << ": " << bin.Mean() << " " << bin.Error() << std::endl;
      ++i;
    }
    std::cout << "*************************" << std::endl;
    std::cout << "resoltuion container calculated with DataContainersStats" << std::endl;
    auto resolution_stats = Sqrt((*container_Qa_Qb * *container_Qa_Qc)/ *container_Qb_Qc);
    resolution_stats.SetSetting(Qn::Stats::Settings::CORRELATEDERRORS);
    i=0;
    for( const auto &bin : resolution_stats  ){
      std::cout << i << ": " << bin.Mean() << " " << bin.MeanError() << std::endl;
      ++i;
    }
    std::cout << "*************************" << std::endl;
    std::cout << "rebinned resoltuion container" << std::endl;
    i=0;
    for( const auto &bin : resolution_rebinned  ){
      std::cout << i << ": " << bin.Mean() << " " << bin.Error() << std::endl;
      ++i;
    }
  } catch (const std::exception& e) {
    std::cout << "Rebin attempt was unsuccessful for resolution container: " << e.what() << std::endl;
  }

  Qn::DataContainer<Qn::StatBin> flow_rebinned;
  try{
    std::cout << "Trying to rebin flow container centrality axis, unite bins in same range" << std::endl;
    flow_rebinned = flow.Rebin({"Centrality", 1, 0.0, 0.40});
    std::cout << "Rebin attempt was successful for flow container" << std::endl;
    std::cout << "rebinned flow container" << std::endl;
    int i=0;
    for( const auto &bin : flow_rebinned  ){
      std::cout << i << ": " << bin.Mean() << " " << bin.Error() << std::endl;
      ++i;
    }
  } catch (const std::exception& e) {
    std::cout << "Rebin attempt was unsuccessful for flow container: " << e.what() << std::endl;
  }

  auto file_out = TFile::Open("result.root", "recreate");
  file_out->cd();

  resolution.Write("resolution");
  resolution_rebinned.Write("resolution_rebinned");
  flow.Write("flow");
  flow_rebinned.Write("flow_rebinned");

  // original containers used for flow calculation
  stat_bins_Qa_Qb.Write("<Qa,Qb>");
  stat_bins_Qa_Qc.Write("<Qa,Qc>");
  stat_bins_Qb_Qc.Write("<Qb,Qc>");
  stat_bins_un_Qa.Write("<un,Qa>");

  file_out->Close();
  return 0;
}
