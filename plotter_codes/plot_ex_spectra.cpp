// ************************************************************************//
// Macro to plot overlaid excitation energy (Ex) spectra from read_tree_27Al.cpp
// This overlays all h_ex_fX histograms (excitation energies for different angles)
// ************************************************************************//

void plot_ex_spectra() {
    gROOT->SetBatch(kTRUE);

    // ====== USER-CONFIGURABLE VARIABLES ======
    // Define all variables you want to change here at the top!
    // This way you only edit the beginning of the file
    
    // TString inputFile = "27Al_cal1.root";      // Input ROOT file
    // TString isotope = "27Al";


    TString isotope = "106Pd";
    TString inputFile = "106Pd_cal3_combined_26th_with_offset.root";      // Input ROOT file
    TString outputPrefix = "plot_ex_shift_26th" + isotope;        // Prefix for output filenames
    // TString inputFile = "106Pd_cal3_combined_27th_with_offset.root";      // Input ROOT file
    // TString outputPrefix = "plot_ex_shift_27th" + isotope;        // Prefix for output filenames

    TString outputDir = "pdfs";                // Output directory
    
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
    // We have 8 front detectors/angles, so we need 8 distinct colors
    // Each color represents a different angle (f = 0 to 7, corresponding to 126-140 degrees)
    // ROOT has predefined colors: kRed, kBlue, kGreen, kBlack, kMagenta, kCyan, etc.
    Color_t colors[8] = {
        kRed, kBlue, kGreen, kMagenta, kCyan, kYellow+2, kOrange, kViolet
    };
    
    
    // ====== CREATING A CANVAS ======
    // TCanvas is your drawing area. Arguments: (name, title, width, height)
    TCanvas *c = new TCanvas("c_ex", "Excitation Energy Spectra Overlay", 1200, 800);
    
    // Optional: Set canvas properties
    c->SetGrid();  // Add grid lines for easier reading
    
    
    // ====== DRAWING HISTOGRAMS WITH OVERLAY ======
    // We iterate through all 8 h_ex_fX histograms (one for each angle/ring)
    
    bool first = true;  // Flag to handle the first histogram differently
    
    for (int f = 0; f < 8; ++f) {
        
        // Build the histogram name: h_ex_f{f}
        char histName[50];
        sprintf(histName, "h_ex_f%d", f);
        
        // Get the histogram from the file
        TH1D *hist = (TH1D*)file->Get(histName);
        
        // Check if histogram exists
        if (!hist) {
            std::cerr << "WARNING: Could not find histogram " << histName << std::endl;
            continue;
        }
        
        // ====== COLOR SELECTION ======
        // Color based on front detector/angle index (f)
        // Each angle gets its own color
        int colorIdx = f;  // Color based on angle
        
        // Set histogram properties
        hist->SetLineColor(colors[colorIdx]);      // Line color
        hist->SetLineWidth(2);                      // Line width for visibility
        hist->SetMarkerColor(colors[colorIdx]);    // Marker color (if using markers)
        // Note: SetFillColor is NOT set so histograms are transparent
        
        // ====== THE DRAW() FUNCTION ======
        // Syntax: hist->Draw("option")
        // 
        // Common draw options:
        // "L"     = Line (default for 1D histograms)
        // "H"     = Histogram (bar chart style)
        // "E"     = Error bars
        // "P"     = Points/markers
        // "SAME"  = Draw on top of previous histogram (CRITICAL for overlay!)
        // "HIST"  = Draw as histogram, not smooth curve
        //
        // You can combine options: "SAME L" = overlay with line style
        
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
    
    
    // ====== CUSTOMIZE THE PLOT ======
    // After drawing, we can modify the axes
    TH1D *firstHist = (TH1D*)file->Get("h_ex_f0");
    
    if (firstHist) {
        // Set axis titles
        firstHist->GetXaxis()->SetTitle("E_{x} (keV)");
        firstHist->GetYaxis()->SetTitle("Counts");
        
        // Set title
        firstHist->SetTitle(Form("Excitation Energy Spectra for All Angles (%s)", isotope.Data()));
        
        // Optional: Set axis ranges (auto-scaling is usually fine)
        // firstHist->GetXaxis()->SetRangeUser(-1000, 20000);
        // firstHist->GetYaxis()->SetRangeUser(0, 100);
    }
    
    
    // ====== LEGEND ======
    // A legend is helpful when overlaying many histograms
    // Show one entry per angle (front detector)
    
    TLegend *leg = new TLegend(0.7, 0.5, 0.95, 0.95);
    leg->SetFillStyle(0);  // Transparent background
    leg->SetBorderSize(1);
    
    for (int f = 0; f < 8; ++f) {
        char histName[50];
        sprintf(histName, "h_ex_f%d", f);
        TH1D *hist = (TH1D*)file->Get(histName);
        if (hist) {
            leg->AddEntry(hist, Form("Ring %d", f), "l");
        }
    }
    leg->Draw();
    
    
    // ====== SAVE THE PLOT ======
    // In C++, you must explicitly create directories before saving files there
    // gSystem is ROOT's system interface - it handles OS-specific operations
    
    // Create the output directory if it doesn't exist
    gSystem->mkdir(outputDir);
    
    // Save in multiple formats for flexibility
    // Use forward slashes (/) even on Windows - ROOT handles this
    // We use Form() to combine strings with variables
    // c->SaveAs(Form("%s/%s_spectra.pdf", outputDir.Data(), outputPrefix.Data()));
    // c->SaveAs(Form("%s/%s_spectra.png", outputDir.Data(), outputPrefix.Data()));
    // c->SaveAs(Form("%s/%s_spectra.C", outputDir.Data(), outputPrefix.Data()));
    
    // Save canvas as a ROOT object file (.root)
    // This allows you to open the plot later and interact with it
    TFile *outputFile = new TFile(Form("%s/%s_spectra.root", outputDir.Data(), outputPrefix.Data()), "RECREATE");
    c->Write();
    outputFile->Close();
    
    std::cout << "\nPlots saved as:" << std::endl;
    // std::cout << "  - " << outputDir << "/" << outputPrefix << "_spectra.pdf" << std::endl;
    // std::cout << "  - " << outputDir << "/" << outputPrefix << "_spectra.png" << std::endl;
    // std::cout << "  - " << outputDir << "/" << outputPrefix << "_spectra.C (ROOT macro)" << std::endl;
    std::cout << "  - " << outputDir << "/" << outputPrefix << "_spectra.root (ROOT object)" << std::endl;
    
    
    // ====== CLEANUP ======
    // Close the file
    file->Close();
    
    // Note: We typically don't delete canvas/histograms in interactive ROOT
    // as ROOT takes ownership, but good practice in compiled code
}
