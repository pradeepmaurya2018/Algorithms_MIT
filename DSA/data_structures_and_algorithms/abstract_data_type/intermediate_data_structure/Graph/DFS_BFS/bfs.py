from data_structures_and_algorithms.abstract_data_type.intermediate_data_structure.Graph.graph_representation.basic_graph_ll import \
    Graph

graph = Graph(5)
graph.addEdge(0, 1)
graph.addEdge(0, 4)
graph.addEdge(1, 2)
graph.addEdge(1, 3)
graph.addEdge(1, 4)
graph.addEdge(2, 3)
graph.addEdge(3, 4)

graph.printGraph()