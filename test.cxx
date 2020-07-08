void test(){
    auto file = TFile::Open("real_data_eff.root");
    Qn::DataContainerStats* container_un_qn;
    Qn::DataContainerStats* container_qn_qn;
    
    file->GetObject("tracks_mdc_RESCALED_wall_sub2_RECENTERED_Q1x_Q1x", container_un_qn);
    file->GetObject("wall_sub1_RECENTERED_wall_sub2_RECENTERED_Q1x_Q1x", container_qn_qn);
    
    Qn::DataContainerStats resolution = Sqrt(*container_qn_qn)*sqrt(2.0);
    Qn::DataContainerStats flow = *container_un_qn / resolution;
    
    auto file_out = TFile::Open("result.root", "recreate");
    file_out->cd();
    
    resolution.Write("resolution");
    flow.Write("flow");
}
