#SRC=pmic_babbling.tex pmic_babbling_noratio.tex

all: pmic_babbling.pdf pmic_babbling_noratio.pdf

pmic_babbling.pdf: pmic_babbling.tex pmic_babbling_noratio.tex
	pdflatex pmic_babbling.tex
	pdflatex pmic_babbling_noratio.tex

clean:
	rm -rf pmic_babbling.pdf
	rm -rf pmic_babbling_noratio.pdf

clean_intermediate:
	rm -rf pmic_babbling_noratio.log
	rm -rf pmic_babbling_noratio.nav
	rm -rf pmic_babbling_noratio.out
	rm -rf pmic_babbling_noratio.snm
	rm -rf pmic_babbling_noratio.vrb
	rm -rf pmic_babbling_noratio.toc
	rm -rf pmic_babbling_noratio.aux
	rm -rf pmic_babbling.log
	rm -rf pmic_babbling.nav
	rm -rf pmic_babbling.out
	rm -rf pmic_babbling.snm
	rm -rf pmic_babbling.vrb
	rm -rf pmic_babbling.toc
	rm -rf pmic_babbling.aux

