#include <DataContainer.hpp>
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

  // Resolution calculation with 3-Sub method
  // All containers in this operation are reference, Sqrt() is unary => result resolution is mergeable
  auto resolution = Sqrt(*container_Qa_Qb * *container_Qa_Qc / *container_Qb_Qc);
  // Flow calculation
  // Observable / Reference => result flow is mergeable
  auto flow = *container_un_Qa / resolution;

  // Appearance 1: Rebin of mergeable container falls with error
  Qn::DataContainerStats flow_rebinned;
  try{
    std::cout << "Trying to rebin flow container centrality axis, halve bins in same range" << std::endl;
    flow_rebinned = flow.Rebin({"Centrality", 4, 0, 40});
    std::cout << "Rebin attempt was successful for flow container" << std::endl;
  } catch (const std::exception& e) {
    std::cout << "Rebin attempt was unsuccessful for flow container: " << e.what() << std::endl;
  }

  // Appearance 2: Rebin of mergeable container seems to be success, but with incorrect result
  Qn::DataContainerStats resolution_rebinned;
  try{
    std::cout << "Trying to rebin resolution container centrality axis, halve bins in same range" << std::endl;
    resolution_rebinned = resolution.Rebin({"Centrality", 4, 0.0, 40.0});
    std::cout << "Rebin attempt was successful for resolution container" << std::endl;
  } catch (const std::exception& e) {
    std::cout << "Rebin attempt was unsuccessful for resolution container: " << e.what() << std::endl;
  }

  auto file_out = TFile::Open("result.root", "recreate");
  file_out->cd();

  resolution.Write("resolution");
  resolution_rebinned.Write("resolution_rebinned");
  flow.Write("flow");
  flow_rebinned.Write("flow_rebinned");

  // original containers used for flow calculation
  container_Qa_Qb->Write("<Qa,Qb>");
  container_Qa_Qc->Write("<Qa,Qc>");
  container_Qb_Qc->Write("<Qb,Qc>");
  container_un_Qa->Write("<un,Qa>");

  file_out->Close();
  return 0;
}
