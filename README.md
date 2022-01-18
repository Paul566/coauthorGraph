# Co-authorship Network of Google Scholar

I investigated the properties of a co-authorship network of Google Scholar. The nodes of this network are Google Scholar
profiles. Two nodes are linked if at least one of the authors has specified the other as their co-author. Furthermore, I
am focusing only on one connected component that contains
[Paul Erdos](https://scholar.google.com/citations?user=cVeVZ1YAAAAJ&hl=en) in it. So, we are dealing with an unweighted
undirected connected graph. The list of co-authors for a given author is located at the right-bottom side of their
Google Scholar profile page. So, it is possible for two authors to be co-authors in real life but don't list each other
as co-authors. For example,
[Terence Tao](https://scholar.google.com/citations?user=TFx_gLQAAAAJ&hl=en&oi=ao) has zero co-authors. But he is listed
as a co-author of [Emmanuel Candes](https://scholar.google.com/citations?user=nRQi4O8AAAAJ&hl=en&oi=sra), so he is not
an isolated node and he belongs to the connected component I focus on. So, the number of edges in this graph is likely
considerably underestimated. <br />

### Basic properties of the network

Number of nodes: 969,559 <br />
Number of edges: 3,873,577 <br />
Maximal Erdos number (the maximal distance from Paul Erdos to some other node): 11

My objective was to find out if the coauthorship graph is decently connected. There are reasons to believe that, for
example, the physicists - non-physicists or Chinese - non-Chinese partition would split the graph into poorly connected
parts. Let's try to minimize the conductance (a.k.a. the Cheeger constant) of the graph h, which measures the '
bottleneckedness' of the graph (defined, for
example, [here](https://orion.math.iastate.edu/butler/PDF/spectra_lecture_3.pdf)). A small conductance means the
presence of two relatively large poorly connected communities. Determining the conductance is an NP-hard problem, so
trying to implement an exact algorithm is a hopeless idea. First, I have implemented two relatively standard approaches.

### Spectral partition

It is known that if we sort the vertices with respect to the coordinates of the eigenvector that corresponds to the
second-smallest eigenvalue of the Laplacian matrix of a graph, and search among the partitions of the form "the one part
is vertices 1..k, another part is vertices k+1..n in the sorted array of vertices", we would get a decent
partition. <br />
Eigen and Spectra packages were used to compute the eigenvalues and the eigenvectors. In our case, the method finds a
partition with conductance 0.0133 and the sizes of parts 37 and 969522. It turned out that there is a community of 37
people that are connected with an outside world with just one edge. That's not exactly what I was looking for. If we
demand the sizes of parts be at least 10,000, we get a partition of conductance 0.141 and sizes of the parts 605255 and 364304. <br />

Since the community of 37 people strongly affects the second eigenvector of the Laplacian matrix, I also tried to
exclude the 37-community and recalculate the second eigenvector of the Laplacian matrix. Manually setting the
coordinates of the vector that would correspond to the vertices from the 37-community to zeros, we got a partition of
the conductance 0.0579 and sizes of the parts 3228 and 966331. This is again not exactly what I wanted to see. If we
limit the search to the partitions with the sizes of the parts greater than 10,000, we would get a partition of
conductance 0.250 and sizes of the parts 318612 and 650947. <br />
There is also a nice property called Cheeger's inequality that allows one to set bounds for the conductance h using the
second-smallest eigenvalue of
the [normalized Laplacian matrix] https://en.wikipedia.org/wiki/Laplacian_matrix#Symmetric_normalized_Laplacian) of the
graph.
<br />
<img src="https://latex.codecogs.com/gif.latex?\frac{\lambda_2}{2}\leq&space;h\leq\sqrt{2\lambda_2}" /> <br />
The first and the second computed eigenvalues are 1.6e-15 and 0.00878701452518. The first value is meant to be exactly
zero, but it differs a bit due to a numerical error. In fact, all eigenvalues of the normalized Laplacian matrix of a
graph are non-negative and there is a zero eigenvalue that corresponds to (sqrt(d<sub>1</sub>), sqrt(d<sub>2</sub>), ...
sqrt(d<sub>n</sub>)) eigenvector, where d<sub>i</sub> are the degrees of the vertices. If the graph is connected (which
is our case), all other eigenvalues are positive.<br />
So, using Cheeger's inequality we have 0.00439 < h < 0.133. <br />
The 37-community instance of partition gives a new upper bound for conductance h, and 0.00439 < h < 0.0133. <br />
When the 37-community is excluded, the second eigenvalue of the normalized Laplacian matrix of the new graph is
0.0093441, so, using Cheeger's inequality, we get the following bounds for the conductance of the co-authorship graph
without the 37-community: 0.00467 < h<sub>-37</sub> < 0.137, which is not much different from the bound on the
conductance of the entire graph. It is worth noting that the exclusion of 37 random people from the network shouldn't
affect the second eigenvalue of the normalized Laplacian matrix significantly, but since the 37 people had large
components of the eigenvector corresponding to them, I was not sure if the exclusion of the 37-community would change
the second eigenvalue a lot.

### Annealing

Another relatively standard general approach is to anneal the problem. The initial partition is random. The step of
annealing is to pick a random vertex and try to move it to another part of the partition. The diameter of the network of
states in this process is 10<sup>6</sup>, which is not very good for annealing, but since there are about 2<sup>10<sup>
6</sup></sup> possible partitions, there is not much we can do to keep both the diameter of the network of possible
states and the number of the adjacent states for a given state relatively small. 10<sup>10</sup> steps of annealing gave
a partition with conductance 0.0422 and sizes of the parts 482538 and 487021. <br />
So far, the best-known instance of partition is cutting off the 37-community, with conductance 0.0133, obtained with
spectral partitioning. The best-known instance of partition with the sizes of the parts that are not small has
conductance 0.0422 obtained with annealing.

### The degree distribution of the graph

![](figures/degHist.png)
This is the plot of the probability of finding a vertex of a given degree for the GS coauthorship graph and Erdos-Renyi
random graph with the same number of vertices and edges. As one can see, the GS coauthorship graph has a huge amount of
leaves, in fact, 35% of all profiles (in the connected component I am focusing on). There is also an increase in the
density of vertices at degree 20, which I can't explain.
