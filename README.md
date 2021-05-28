# Co-authorship Network of Google Scholar
I investigated the properties of a co-authorship network of Google Scholar. The nodes of this network are Google Scholar profiles. Two nodes are linked if at least 
one of the authors has specified the other as their co-author. Furthermore, I am focusing only on one connected component that contains 
[Paul Erdos](https://scholar.google.com/citations?user=cVeVZ1YAAAAJ&hl=en) in it. So, we 
are dealing with an unweighted undirected connected graph. The list of co-authors for a given author is located at the right-bottom side of their Google Scholar 
profile page. So, it is possible for two authors to be co-authors in real life but don't list each other as co-authors. For example, 
[Terence Tao](https://scholar.google.com/citations?user=TFx_gLQAAAAJ&hl=en&oi=ao) has zero
co-authors. But he is listed as a co-author of [Emmanuel Candes](https://scholar.google.com/citations?user=nRQi4O8AAAAJ&hl=en&oi=sra), so he is not 
an isolated node and he belongs to the connected component I focus on. <br />
The process of breadth-first search is still running, but the properties of the currently observable part of the network are given below.

### Basic properties of the network
Number of nodes: 245660 <br />
Number of edges: 559700 <br />
Maximal Erdos number (the maximal distance from Paul Erdos to some other node):

### Properties connected to the spectrum of the graph
In this section, I focus on the spectrum of the Laplace matrix of the graph. Eigen and Spectra packages are used. 
It is known that all eigenvalues of the Laplace matrix of a graph are non-negative and there is a zero eigenvalue that corresponds to (1, 1, 1, ... 1) eigenvector. 
If the graph is connected (which is our case), all other eigenvalues are positive. Unfortunately, computing all the eigenvalues of a given matrix NxN costs 
N^3 operations, which is far too long in our case, so I only compute the spectral gap of the graph, which is the difference between the first and the 
second eigenvalues (equals to the second eigenvalue itself since the first one is zero). The first and the second computed eigenvalues are ... and ... <br />
Next, I try to estimate the Cheeger constant h, which measures the 'bottleneckedness' of the graph. In our case, a small Cheeger constant would mean the 
presence of two large poorly connected communities. Determining the Cheeger constant exactly is an NP-hard problem, meaning no one will ever find it for 
the Google Scholar co-authorship graph. But we can set some boundaries for h. First, use Cheeger's inequality
lambda2/2 <= h <= sqrt(2 lambda2) to obtain ... <= h <= ...
