#include <DataContainer.hpp>
#include <TFile.h>

int main(int n_args, char **args) {
  assert(n_args==2);
  std::string file_name=args[1];
  auto file =
      TFile::Open(file_name.data());
  assert(file);
  // u1Q1 correlation
  // Axis: {"Centrality", 8, 0, 40}, {"mdc_vtx_tracks_pT", 16, 0, 1.6}, {"y_cm", 15, -0.75, 0.75}
  Qn::DataContainerStats *container_un_qn;
  // Correlations needed for resolution calculation
  // All are single-axis {"Centrality", 8, 0, 40}
  Qn::DataContainerStats *container_w1_w2;
  Qn::DataContainerStats *container_w2_w3;
  Qn::DataContainerStats *container_w1_w3;

  file->GetObject("u_RESCALED_W1_RECENTERED_Q1x_Q1x", container_un_qn);
  file->GetObject("W1_RECENTERED_W2_RECENTERED_Q1x_Q1x", container_w1_w2);
  file->GetObject("W1_RECENTERED_W3_RECENTERED_Q1x_Q1x", container_w1_w3);
  file->GetObject("W2_RECENTERED_W3_RECENTERED_Q1x_Q1x", container_w2_w3);

  auto resolution = Sqrt(*container_w1_w2 * *container_w1_w3 / *container_w2_w3); // Simple 3-Sub method
  auto flow_3d = *container_un_qn / resolution; // Impossible to make any projection or rebin even if bins are just cut off
  try{
    flow_3d = flow_3d.Rebin({"Centrality", 1, 20, 25});
    flow_3d = flow_3d.Rebin({"mdc_vtx_tracks_pT", 1, 1.0, 1.1});
    flow_3d = flow_3d.Projection({"y_cm"});
  } catch (const std::exception& e) {
    std::cout << "main() 27-28 lines catched: " << e.what() << std::endl;
  }

  // Only centrality projection of flow is clickable, how to look at the differential flow?
  auto un_qn_centrality = container_un_qn->Rebin({"y_cm", 1, -0.25, -0.15});
  un_qn_centrality = un_qn_centrality.Projection({"Centrality"});
  auto flow_1d = un_qn_centrality / resolution;

  auto file_out = TFile::Open("result.root", "recreate");
  file_out->cd();

  resolution.Write("resolution");
  flow_3d.Write("flow_all_axis");
  flow_1d.Write("flow_centrality");
  file_out->Close();
  return 0;
}
