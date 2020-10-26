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
  Qn::DataContainerStatCollect *container_un_Qa;
  // Correlations needed for resolution calculation
  // All are single-axis {"Centrality", 8, 0, 40}
  Qn::DataContainerStatCollect *container_Qa_Qb;
  Qn::DataContainerStatCollect *container_Qb_Qc;
  Qn::DataContainerStatCollect *container_Qa_Qc;

  // reading containers from file
  file->GetObject("/uQ/SP/u_RESCALED.W1_RESCALED.x1x1",
                  container_un_Qa); // Observable, 3-axes: {"Centrality", 8, 0, 40}, {"mdc_vtx_tracks_pT", 16, 0, 1.6}, {"y_cm", 15, -0.75, 0.75}
  file->GetObject("/QQ/SP/W2_RESCALED.W1_RESCALED.x1x1",
      container_Qa_Qb); // Reference, single-axis {"Centrality", 8, 0, 40}
  file->GetObject("/QQ/SP/W3_RESCALED.W1_RESCALED.x1x1",
      container_Qa_Qc); // Reference, single-axis {"Centrality", 8, 0, 40}
  file->GetObject("/QQ/SP/W3_RESCALED.W2_RESCALED.x1x1",
      container_Qb_Qc); // Reference, single-axis {"Centrality", 8, 0, 40}

  // Copying stats containers to stat bins
  Qn::DataContainer<Qn::StatCalculate> stat_bins_un_Qa( *container_un_Qa );
  Qn::DataContainer<Qn::StatCalculate> stat_bins_Qa_Qb( *container_Qa_Qb );
  Qn::DataContainer<Qn::StatCalculate> stat_bins_Qa_Qc( *container_Qa_Qc );
  Qn::DataContainer<Qn::StatCalculate> stat_bins_Qb_Qc( *container_Qb_Qc );

  // Resolution calculation with 3-Sub method
  auto resolution = Sqrt(stat_bins_Qa_Qb * stat_bins_Qa_Qc / stat_bins_Qb_Qc)*sqrt(2.0);
  // Flow calculation
  auto flow = stat_bins_un_Qa*2.0 / resolution;

  auto file_out = TFile::Open("result.root", "recreate");

  file_out->cd();
  resolution.Write("resolution_bootstrap");
  resolution.SetErrors(Qn::StatCalculate::ErrorType::PROPAGATION);
  resolution.Write("resolution_propagation");
  flow.Write("flow_bootstrap");
  flow.SetErrors(Qn::StatCalculate::ErrorType::PROPAGATION);
  flow.Write("flow_propagation");

  // original containers used for flow calculation
  stat_bins_Qa_Qb.Write("<Qa,Qb>");
  stat_bins_Qa_Qc.Write("<Qa,Qc>");
  stat_bins_Qb_Qc.Write("<Qb,Qc>");
  stat_bins_un_Qa.Write("<un,Qa>");

  file_out->Close();
  return 0;
}
