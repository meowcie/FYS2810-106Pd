// ************************************************************************//
// Macro to plot overlaid E+ΔE spectra from read_tree_27Al.cpp
// This demonstrates ROOT plotting fundamentals
// ************************************************************************//

void plot_e_de_spectra() {
    gROOT->SetBatch(kTRUE);
    
    // ====== USER-CONFIGURABLE VARIABLES ======
    // Define all variables you want to change here at the top!
    // This way you only edit the beginning of the file
    
    // TString filename = "27Al_cal1";      // Input ROOT file
    // TString isotope = "27Al";

    
    
    // TString filename = "106Pd_cal2";      // Input ROOT file
    // TString filename = "106Pd_cal_combined";      // Input ROOT file
    // TString filename = "106Pd_cal_combined_26th";      // Input ROOT file
    TString filename = "106Pd_cal_combined_27th";      // Input ROOT file
    TString isotope = "106Pd";
    
    TString inputFile = filename + ".root";  
    TString outputDir = "pdfs";                // Output directory
    TString outputPrefix = "plot_e_de" + isotope;        // Prefix for output filenames
    
    // ====== END OF CONFIGURATION ======
    
    
    // ====== OPENING THE ROOT FILE ======
    // This opens the file in READ mode (default)
    // Now we use the variable instead of hardcoded filename
    
    TFile *file = new TFile(inputFile, "READ");
    
    // Always check if file opened successfully!
    if (!file || file->IsZombie()) {
        std::cerr << "ERROR: Could not open file " << inputFile << std::endl;
        return;
    }
    std::cout << "File opened successfully!" << std::endl;
    

    // ====== COLOR PALETTE ======
    // We have 8 back detectors, so we need 8 distinct colors
    // ROOT has predefined colors: kRed, kBlue, kGreen, kBlack, kMagenta, kCyan, etc.
    Color_t colors[8] = {
        kRed, kBlue, kGreen, kMagenta, kCyan, kYellow+2, kOrange, kViolet
    };
    
    
    // ====== CREATING CANVASES ======
    // We'll create 8 canvases, one for each front detector (f = 0 to 7)
    // Each canvas will show all 8 back detectors for that front strip
    TCanvas *canvases[8] = {NULL};
    
    // Create 8 canvases arranged in a 2x4 grid (optional)
    for (int f = 0; f < 8; ++f) {
        char canvasName[50];
        sprintf(canvasName, "c_e_de_f%d", f);
        canvases[f] = new TCanvas(canvasName, Form("E + #DeltaE for Front Detector %d", f), 1200, 800);
        canvases[f]->SetGrid();
    }
    
    
    // ====== DRAWING HISTOGRAMS ======
    // Loop through each front detector (f)
    for (int f = 0; f < 8; ++f) {
        // Switch to the current canvas
        canvases[f]->cd();
        
        bool first = true;  // Flag to handle the first histogram differently
        
        // For each front detector, loop through all back detectors (b)
        for (int b = 0; b < 8; ++b) {
            
            // Build the histogram name: h_e_de_b{b}f{f}
            char histName[50];
            sprintf(histName, "h_e_de_b%df%d", b, f);
            
            // Get the histogram from the file
            TH1D *hist = (TH1D*)file->Get(histName);
            
            // Check if histogram exists
            if (!hist) {
                std::cerr << "WARNING: Could not find histogram " << histName << std::endl;
                continue;
            }
            
            // ====== COLOR SELECTION ======
            // Use back detector index (b) for color, so all histograms with same b have same color
            // across different front detectors
            int colorIdx = b;  // Color based on back detector
            
            // Set histogram properties
            hist->SetLineColor(colors[colorIdx]);      // Line color
            hist->SetLineWidth(2);                      // Line width for visibility
            hist->SetMarkerColor(colors[colorIdx]);    // Marker color (if using markers)
            // Note: SetFillColor is NOT set so histograms are transparent
            
            if (first) {
                // First histogram: draw normally to set up the axes
                hist->Draw();
                first = false;
            } else {
                // All subsequent histograms: use "SAME" to overlay
                hist->Draw("SAME");
            }
            
            std::cout << "Drew " << histName << " with color " << colors[colorIdx] << std::endl;
        }
        
        // ====== CUSTOMIZE EACH PLOT ======
        // Get first histogram for this front detector
        char firstHistName[50];
        sprintf(firstHistName, "h_e_de_b0f%d", f);
        TH1D *firstHist = (TH1D*)file->Get(firstHistName);
        
        if (firstHist) {
            // Set axis titles
            firstHist->GetXaxis()->SetTitle("E_{back} + #Delta E_{front} (keV)");
            firstHist->GetYaxis()->SetTitle("Counts");
            
            // Set title specific to this front detector
            firstHist->SetTitle(Form("E + #Delta E Spectra - Front Detector %d (%s)", f, filename.Data()));
            
            // Optional: Set axis ranges (auto-scaling is usually fine)
            // firstHist->GetXaxis()->SetRangeUser(0, 5000);
            // firstHist->GetYaxis()->SetRangeUser(0, 600); // for 106Pd
        }
        
        // ====== LEGEND FOR THIS PLOT ======
        TLegend *leg = new TLegend(0.1, 0.5, 0.30, 0.95);
        leg->SetFillStyle(0);  // Transparent background
        leg->SetBorderSize(1);
        
        for (int b = 0; b < 8; ++b) {
            char histName[50];
            sprintf(histName, "h_e_de_b%df%d", b, f);
            TH1D *hist = (TH1D*)file->Get(histName);
            if (hist) {
                leg->AddEntry(hist, Form("Back det %d", b), "l");
            }
        }
        leg->Draw();
    }
    
    
    // ====== SAVE THE PLOTS ======
    // In C++, you must explicitly create directories before saving files there
    // gSystem is ROOT's system interface - it handles OS-specific operations
    
    // Create the output directory if it doesn't exist
    gSystem->mkdir(outputDir);
    
    // Create a combined ROOT file to store all canvases
    TFile *combinedFile = new TFile(Form("%s/%s%s_all_spectra.root", outputDir.Data(), outputPrefix.Data(), filename.Data()), "RECREATE");
    
    // Save all 8 canvases (one for each front detector)
    for (int f = 0; f < 8; ++f) {
        canvases[f]->cd();  // Make sure we're working with the correct canvas
        
        // Save in multiple formats for flexibility
        // canvases[f]->SaveAs(Form("%s/%s_f%d_spectra.pdf", outputDir.Data(), outputPrefix.Data(), f));
        // canvases[f]->SaveAs(Form("%s/%s_f%d_spectra.png", outputDir.Data(), outputPrefix.Data(), f));
        // canvases[f]->SaveAs(Form("%s/%s_f%d_spectra.C", outputDir.Data(), outputPrefix.Data(), f));
        
        // Write canvas to the combined ROOT file
        canvases[f]->Write();
    }
    
    // Close the combined ROOT file
    combinedFile->Close();
    
    std::cout << "\nPlots saved as:" << std::endl;
    std::cout << "  Combined ROOT file: " << outputDir << "/" << outputPrefix << "_" <<  filename.Data() <<"_all_spectra.root" << std::endl;
    for (int f = 0; f < 8; ++f) {
        std::cout << "  Front detector " << f << ":" << std::endl;
        // std::cout << "    - " << outputDir << "/" << outputPrefix << "_f" << f << "_spectra.pdf" << std::endl;
        // std::cout << "    - " << outputDir << "/" << outputPrefix << "_f" << f << "_spectra.png" << std::endl;
        // std::cout << "    - " << outputDir << "/" << outputPrefix << "_f" << f << "_spectra.C (ROOT macro)" << std::endl;
    }
    
    
    // ====== CLEANUP ======
    // Close the file
    file->Close();
    
    // Note: We typically don't delete canvas/histograms in interactive ROOT
    // as ROOT takes ownership, but good practice in compiled code
}
