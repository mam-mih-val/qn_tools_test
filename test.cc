#include <DataContainer.hpp>
#include <TFile.h>
#include <TCanvas.h>

int main(int n_args, char **args) {
  assert(n_args==2);
  std::string file_name=args[1];
  auto file =
      TFile::Open(file_name.data());
  assert(file);

  Qn::DataContainerStatCollect *container;

  // Reading the object from the file:
  // 3 axes: centrality (12, 0, 60); y (15, -0.75, 0.75); pT(10, 0.29375, 0.35625, 0.41875, 0.48125, 0.54375, 0.61875, 0.70625, 0.81875, 1.01875, 2) -- Non-uniform binning
  file->GetObject( "uQ/reco/pid_protons_RESCALED.psi_rp_PLAIN.x1x1", container );

  auto v1_pT = container->Select( {"event_header_selected_tof_rpc_hits_centrality", 2, 20, 30} ); // Selection 2 bins in centrality axis
  v1_pT = v1_pT.Select({"mdc_vtx_tracks_rapidity_rapidity", 1, -0.25+0.74, -0.15+0.74}); // Selection of 1 bin on rapidity-axis
  v1_pT = v1_pT.Projection( {"mdc_vtx_tracks_rapidity_pT"} ); // projection to pT-axis
  v1_pT.Print();
  auto v1_pT_selected = v1_pT.Select({"mdc_vtx_tracks_rapidity_pT", 9, 0.29375, 2.0}); // Selection of 9 bins on pT axis
  v1_pT_selected.Print(); // Bin-edges changed
  try {
    auto v1_pT_rebinned = v1_pT.Rebin({"mdc_vtx_tracks_rapidity_pT", 9, 0.29375, 2.0});
    v1_pT_rebinned.Print();
  } catch (std::exception& e) {
    std::cout << "Rebin was not successed: " << e.what() << std::endl;
  }
  auto canv = new TCanvas("canv", "");
  canv->cd();
  auto v1_pT_graph = Qn::ToTGraph( v1_pT );
  v1_pT_graph->SetTitle( "All bins" );
  v1_pT_graph->SetLineColor( kRed );
  v1_pT_graph->Draw();
  auto v1_pT_selected_graph = Qn::ToTGraph( v1_pT_selected );
  v1_pT_selected_graph->SetLineColor( kBlue );
  v1_pT_selected_graph->SetTitle("Selected 9 bins");
  v1_pT_selected_graph->Draw("same");
  gPad->BuildLegend();
  auto file_out = TFile::Open("result.root", "recreate");
  canv->Write();
  v1_pT.Write("oringinal_bins");
  v1_pT_selected.Write("selected_bins");
  file_out->cd();

  file_out->Close();
  return 0;
}
