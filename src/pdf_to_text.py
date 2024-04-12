import fitz  # Import the PyMuPDF library for working with PDF files
import sys   # Import the sys module for accessing command-line arguments
import os    # Import the os module for operating system functionalities

# Function to convert PDF file to text
def pdf_to_text(src, dest):
    # Open the PDF file
    pdf = fitz.open(src)
    try:
        # Open the destination file in write mode (create if not exists)
        with open(dest, "w+") as f:
            # Iterate through each page of the PDF
            for i in range(0, pdf.page_count):
                # Extract text from the current page and write it to the destination file
                f.write(pdf.load_page(i).get_text().replace("\t", " "))  # Replace tabs with spaces
            return 0
    except Exception:
        return 1  # Return error signal if an exception occurs during file operation

# Entry point of the program
if __name__ =="__main__":
    # Check if the correct number of command-line arguments is provided
    if (len(sys.argv)!=3):
        print(-1)  # Error code -1 if incorrect usage
    else:
        src = sys.argv[1]   # Source PDF file path (1st argument)
        dest = sys.argv[2]  # Destination text file path (2nd argument)

        # Check if the source PDF file exists
        if (os.path.exists(src)):
            signal = pdf_to_text(src, dest)  # Convert PDF to text and get the return signal
            print(signal)  # Print the return signal
        else:
            print(2)  # Error code 2 if source PDF file does not exist
