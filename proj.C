// In a file called proj.C
// Usage: proj() to draw all detectors overlaid
//        proj(5) to draw just detector 5
//        proj("5,20,23") to draw detectors 5, 20, and 23 overlaid
//        proj("", "5,20,23") to draw all detectors EXCEPT 5, 20, and 23
void proj(TString detectors = "", TString exclude = "") {
    
    // Clear previous histograms to avoid duplicate symbol errors on re-run
    gROOT->GetListOfCanvases()->Delete();
    
    // Parse detector list
    std::vector<int> det_list;
    std::vector<int> exclude_list;
    
    // Parse exclude list first
    if (exclude != "") {
        TObjArray *tokens = exclude.Tokenize(",");
        for (int i = 0; i < tokens->GetEntries(); ++i) {
            TString token = ((TObjString*)tokens->At(i))->GetString();
            token.ReplaceAll(" ", "");  // Remove spaces
            exclude_list.push_back(token.Atoi());
        }
        delete tokens;
    }
    
    if (detectors == "") {
        // Empty string: use all detectors (0-31) except those in exclude_list
        for (int i = 0; i < 32; ++i) {
            bool is_excluded = false;
            for (int excl : exclude_list) {
                if (i == excl) {
                    is_excluded = true;
                    break;
                }
            }
            if (!is_excluded) {
                det_list.push_back(i);
            }
        }
    } else {
        // Parse comma-separated list
        TObjArray *tokens = detectors.Tokenize(",");
        for (int i = 0; i < tokens->GetEntries(); ++i) {
            TString token = ((TObjString*)tokens->At(i))->GetString();
            token.ReplaceAll(" ", "");  // Remove spaces
            det_list.push_back(token.Atoi());
        }
        delete tokens;
    }
    
    // If single detector, just draw it simply
    if (det_list.size() == 1) {
        int det = det_list[0];
        TH1D *proj = (TH1D*)h_LaBr3_energy->ProjectionX(
            Form("det%d", det), det, det, "d"
        );
        proj->Draw();
        return;
    }
    
    // Otherwise, overlay all detectors
    TCanvas *c = new TCanvas("c_proj", "LaBr3 Detector Projections Overlay", 1200, 700);
    c->SetGrid();
    
    // Color palette for up to 32 detectors
    Color_t colors[32] = {
        kRed, kBlue, kGreen, kMagenta, kCyan, kYellow+2, kOrange, kViolet,
        kRed+1, kBlue+1, kGreen+1, kMagenta+1, kCyan+1, kYellow+1, kOrange+1, kViolet+1,
        kRed-5, kBlue-5, kGreen-5, kMagenta-5, kCyan-5, kYellow-5, kOrange-5, kViolet-5,
        kRed-9, kBlue-9, kGreen-9, kMagenta-9, kCyan-9, kYellow-9, kOrange-9, kViolet-9
    };
    
    // Create legend
    TLegend *leg = new TLegend(0.7, 0.5, 0.95, 0.95);
    leg->SetFillStyle(0);
    leg->SetBorderSize(1);
    
    // Store histograms so they don't get deleted
    std::vector<TH1D*> projections;
    
    // First, create and store all projections
    for (int i = 0; i < det_list.size(); ++i) {
        int det = det_list[i];
        TH1D *proj = (TH1D*)h_LaBr3_energy->ProjectionX(
            Form("det%d_overlay_%d", det, i), det, det, "d"
        );
        
        if (!proj || proj->GetEntries() == 0) continue;
        
        proj->SetLineColor(colors[det % 32]);
        proj->SetLineWidth(2);
        projections.push_back(proj);
    }
    
    // Now draw all stored histograms
    for (size_t i = 0; i < projections.size(); ++i) {
        TH1D *proj = projections[i];
        int det = det_list[i];
        
        if (i == 0) {
            proj->Draw();
            proj->GetXaxis()->SetTitle("Energy (keV)");
            proj->GetYaxis()->SetTitle("Counts");
            proj->SetTitle("LaBr3 Detector Energy Projections");
        } else {
            proj->Draw("SAME");
        }
        
        leg->AddEntry(proj, Form("Det %d", det), "l");
    }
    
    leg->Draw();
}