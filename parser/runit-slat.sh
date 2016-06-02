#! /bin/sh

export CLASSPATH=".:/usr/local/lib/antlr-4.5.2-complete.jar:$CLASSPATH"
alias grun='java org.antlr.v4.gui.TestRig'
alias antlr4='java -jar /usr/local/lib/antlr-4.5.2-complete.jar'

antlr4 -Dlanguage=Python3 slatLexer.g4 && \
antlr4 -Dlanguage=Python3 slatParser.g4 && \
./test_slat.py


cat<<EOF > testscript.org
#+Title:     SLAT Language
#+AUTHOR:    Michael Gauland
#+EMAIL:     michael.gauland@canterbury.ac.nz
#+DATE:      {{{time(%Y-%m-%d %H:%M)}}}
#+DESCRIPTION: 
#+KEYWORDS:
#+LANGUAGE:  en
#+OPTIONS:   H:6 num:t toc:nil \n:nil @:t ::t |:t ^:{} -:t f:t *:t <:t
#+OPTIONS:   TeX:dvipng LaTeX:dvipng skip:nil d:nil todo:t pri:nil tags:not-in-toc
#+OPTIONS:   timestamp:t email:t
#+OPTIONS:   ':t
#+EXPORT_SELECT_TAGS: export
#+EXPORT_EXCLUDE_TAGS: noexport
#+LaTeX_CLASS: article
#+LaTeX_CLASS_OPTIONS: [a4paper]
#+LATEX_HEADER: \usepackage{unicode-math}
#+LaTex_header: \usepackage{epstopdf}
#+LATEX_HEADER: \usepackage{register}
#+LATEX_HEADER: \usepackage{bytefield}
#+LATEX_HEADER: \usepackage{parskip}
#+LATEX_HEADER: \usepackage{tabulary}
#+LATEX_HEADER: \usepackage[section]{placeins}
#+LATEX_HEADER: \usepackage[htt]{hyphenat}
#+LATEX_HEADER: \setlength{\parindent}{0pt}
#+LATEX_HEADER: \lstset{extendedchars=true}
#+LATEX_HEADER: \lstset{keywordstyle=\color{blue}\bfseries}
#+LATEX_HEADER: \lstset{frame=shadowbox}
#+LATEX_HEADER: \lstset{basicstyle=\ttfamily}
#+LATEX_HEADER: \definecolor{mygray}{gray}{0.8}
#+LATEX_HEADER: \lstset{rulesepcolor=\color{mygray}}
#+LATEX_HEADER: \lstdefinelanguage{sh}{rulecolor=\color{green},rulesepcolor=\color{mygray},
#+LATEX_HEADER: frameround=ffff,backgroundcolor=\color{white}}
#+LATEX_HEADER: \lstdefinelanguage{fundamental}{basicstyle=\ttfamily\scriptsize,
#+LATEX_HEADER: rulesepcolor=\color{cyan},frameround=tttt,backgroundcolor=\color{white},
#+LATEX_HEADER: breaklines=true}
#+LATEX_HEADER: \lstdefinelanguage{command}{basicstyle=\ttfamily\scriptsize,
#+LATEX_HEADER: rulesepcolor=\color{blue},frameround=tttt,backgroundcolor=\color{white},
#+LATEX_HEADER: breaklines=true}
#+LATEX_HEADER: \usepackage{pst-circ}
#+LATEX_HEADER: \usepackage[hang,small,bf]{caption}
#+LATEX_HEADER: \setlength{\captionmargin}{20pt}
#+LINK_UP:   
#+LINK_HOME: 
#+XSLT:
#+STARTUP: overview
#+STARTUP: align
#+STARTUP: noinlineimages
#+PROPERTY: cache yes
#+PROPERTY: exports results
#+MACRO: keyword src_slat[:exports code]{$1}
#+LATEX_HEADER: \lstdefinelanguage{slat}{keywordstyle={\color{blue}},
#+LATEX_HEADER: morekeywords={option,outputdir,set,title,detfn,probfn,powerlaw,lognormal,
#+LATEX_HEADER: fragfn,im,edp,lossfn,comgrp,message,print,integration,recorder,maq,create,
#+LATEX_HEADER: append,cols,mu,sigma,at,analyze,imrate,edpim,edprate,dsedp,dsim,dsrate,
#+LATEX_HEADER: upper_cost, lower_cost, lower_n, upper_n, cost, disp, mean_uncert,
#+LATEX_HEADER: var_uncert, median_X, mean_X, mu_lnX, sd_X, sigma_lnX,
#+LATEX_HEADER: placement_type, spread_type,
#+LATEX_HEADER: lossds,lossedp,lossim,stdfunc,db }
#+LATEX_HEADER: ,sensitive=true
#+LATEX_HEADER: ,morecomment=[l]{\#}
#+LATEX_HEADER: ,morestring=[b]"
#+LATEX_HEADER: ,morestring=[b]'
#+LATEX_HEADER: ,commentstyle={\itshape\color{gray}}
#+LATEX_HEADER: ,identifierstyle={\color{red}}
#+LATEX_HEADER: ,moredelim=[is][\ttfamily]{|}{|}
#+LATEX_HEADER: ,moredelim=[is][\ttfamily\color{cyan}]{$$}{$$}
#+LATEX_HEADER: ,morecomment=[s][\itshape\color{magenta}]{<}{>}
#+LATEX_HEADER: }
#+LATEX_HEADER: \lstdefinelanguage[ANSI]{C}{identifierstyle={\color{magenta}},
#+LATEX_HEADER:     morekeywords={exp, log, sqrt}
#+LATEX_HEADER: }
#+PROPERTY: header-args:fundamental :exports code :eval never
#+PROPERTY: header-args:command :exports code :eval never

#+BEGIN_SRC sh :session SLAT-LANG :exports results :results output
export CLASSPATH=".:/usr/local/lib/antlr-4.5.2-complete.jar:$CLASSPATH"
alias grun='java org.antlr.v4.gui.TestRig'
alias antlr4='java -jar /usr/local/lib/antlr-4.5.2-complete.jar'
if test $(basename $(pwd)) = "test_cases"; then cd ..; fi
antlr4 slat.g4 && javac slat*.java
mkdir -p test_cases/diagrams
rm -f test_cases/diagrams/*.ps
#+END_SRC 
* One-Liners
EOF
#LINES="test_cases/Title.lines test_cases/detfn.lines test_cases/probfn.lines test_cases/relationships.lines test_cases/fragility.lines"
#for f in $LINES; do
for f in test_cases/*.lines; do
    filename=$(basename $f)
    fileroot=${filename%%.lines}
    echo \*\* $fileroot >> testscript.org
    n=0
    line_number=0
    cat $f | while read line; do
	line_number=$((line_number+1))
	if echo "$line" | grep -qv ^%%; then
	    if echo $line | grep -qv ^\#; then
		if [ -n "$line" ]; then
		    n=$((n+1))
		    testname=${fileroot}_$(printf %03d $n)
		    psfile=${testname}.ps
		    echo \*\*\* \#$n >> testscript.org
		    cat<<EOF >> testscript.org
    #+BEGIN_SRC command
    $(head -$line_number $f | tail -1)
    #+END_SRC

    #+NAME: $testname
    #+BEGIN_SRC sh :session SLAT-LANG :exports results :results output :wrap SRC fundamental
    head -$line_number $f | tail -1 | grun slat script -ps test_cases/diagrams/$psfile 2>&1
    #+END_SRC  

    #+ATTR_LaTeX: :width \textwidth*3/4 :placement [h!bt]
    file:test_cases/diagrams/$psfile

    #+RESULTS: $testname

    #+BEGIN_SRC sh :session SLAT-LANG :exports results :results output :wrap SRC fundamental
    echo
    head -$line_number $f | tail -1 | grun slat script -tree 2>&1
    #+END_SRC  

    \clearpage  
EOF
		fi
	    fi
	fi
    done
done   

echo \* Scripts >> testscript.org

for f in test_cases/*.slat; do 
    echo $f
    slatfile=$(basename $f)
    testname=${slatfile%%\.slat}
    psfile=${slatfile%%slat}ps
    echo \*\* $slatfile >> testscript.org
    cat<<EOF >> testscript.org
   #+BEGIN_SRC command
   $(cat $f)
   #+END_SRC

   #+NAME: $testname
   #+BEGIN_SRC sh :session SLAT-LANG :exports results :results output :wrap SRC fundamental
   grun slat script -ps test_cases/diagrams/$psfile test_cases/$slatfile 2>&1
   #+END_SRC  

   #+ATTR_LaTeX: :width \textwidth*3/4 :placement [h!bt]
   file:test_cases/diagrams/$psfile

   #+RESULTS: $testname

   #+NAME: $testname
   #+BEGIN_SRC sh :session SLAT-LANG :exports results :results output :wrap SRC fundamental
   grun slat script -tree test_cases/$slatfile 2>&1
   #+END_SRC  

   \clearpage  
EOF
done   


time emacs testscript.org --user $USER --batch -f org-latex-export-to-pdf --kill
okular testscript.pdf
