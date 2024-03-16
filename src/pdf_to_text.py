import fitz
import sys
import os

def pdf_to_text(src, dest):
    pdf  = fitz.open(src)
    try:
        with open(dest, 'w+') as f:
            for i in range(0, pdf.page_count):
                f.write(pdf.load_page(i).get_text().replace("\t", " "))
        return 0
    except Exception:
        return 1


if __name__=="__main__":
    if (len(sys.argv)!=3):
        print(1)
    else:
        src = sys.argv[1]
        dest = sys.argv[2]
        if os.path.exists(src):
            signal = pdf_to_text(src, dest)
            print(signal)
        else:
            print(-1)


