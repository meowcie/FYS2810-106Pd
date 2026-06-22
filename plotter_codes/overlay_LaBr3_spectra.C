void overlay_LaBr3_spectra() {
    // ==== Settings ====
    const char *inputFileName  = "27Al_cal1.root";
    const char *outputFileName = "LaBr3_overlay.root";
    const int   nDetMax        = 32;  // adjust to how many LaBr3 detectors you have

    // ==== Open input file ====
    TFile *f = new TFile(inputFileName);
    if (!f || f->IsZombie()) {
        std::cerr << "Could not open input file: " << inputFileName << std::endl;
        return;
    }

    TH2D *h2 = (TH2D*)f->Get("h_LaBr3_energy");
    if (!h2) {
        std::cerr << "Could not find h_LaBr3_energy in file." << std::endl;
        f->Close();
        return;
    }

    // ==== Prepare output file ====
    TFile *fout = new TFile(outputFileName, "RECREATE");
    if (!fout || fout->IsZombie()) {
        std::cerr << "Could not create output file: " << outputFileName << std::endl;
        f->Close();
        return;
    }

    // ==== Canvas for overlay ====
    TCanvas *c = new TCanvas("c_LaBr3_overlay", "LaBr3 energy overlay", 800, 600);
    c->SetLogy();

    // Colors
    int colors[] = {kBlack, kRed, kBlue, kGreen+2, kMagenta, kCyan+1, kOrange+7,
                    kGray+2, kViolet, kAzure+2, kPink+7, kTeal+3};
    int nColors = sizeof(colors)/sizeof(colors[0]);

    std::vector<TH1D*> proj;  // store projections so we can write them out
    bool first = true;

    // ==== Loop over detectors and project ====
    for (int det = 0; det < nDetMax; ++det) {
        // y-axis bin corresponding to this detector
        // You filled with labr_ID[j]-1 (0..31)
        int binY = h2->GetYaxis()->FindBin(det + 0.5);

        TString hname;
        hname.Form("h_LaBr3_det%d", det);
        TH1D *h1 = h2->ProjectionX(hname, binY, binY); // projection for this detector

        // Skip empty spectra
        if (!h1 || h1->GetEntries() == 0) {
            if (h1) delete h1;
            continue;
        }

        proj.push_back(h1);

        int col = colors[det % nColors];
        h1->SetLineColor(col);
        h1->SetLineWidth(1);
        h1->SetTitle("LaBr3 energy spectra (overlay)");

        if (first) {
            h1->GetXaxis()->SetTitle("E_{LaBr3} (keV)");
            h1->GetYaxis()->SetTitle("Counts");
            h1->Draw("HIST");
            first = false;
        } else {
            h1->Draw("HIST SAME");
        }
    }

    // Legend with detector names
    TLegend *leg = new TLegend(0.7, 0.6, 0.9, 0.9);
    for (auto h : proj) {
        leg->AddEntry(h, h->GetName(), "l");
    }
    leg->Draw();
    c->Update();

    // ==== Write projections and canvas to output file ====
    fout->cd();

    // Write each projected spectrum
    for (auto h : proj) {
        h->Write();  // name is e.g. "h_LaBr3_det0", "h_LaBr3_det1", ...
    }

    // Write the canvas with the overlay
    c->Write("c_LaBr3_overlay");

    fout->Close();
    f->Close();

    std::cout << "Saved LaBr3 projections and overlay canvas to "
              << outputFileName << std::endl;
}
