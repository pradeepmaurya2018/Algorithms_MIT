import matplotlib.pyplot as plt
import networkx as nx

class DraggableGraph:
    def __init__(self, G, pos=None, weighted=False):
        self.G = G
        self.pos = pos if pos else nx.spring_layout(G, seed=42)
        self.weighted = weighted
        self.node_points = None
        self.dragging_node = None

    def connect(self):
        self.cid_press = self.fig.canvas.mpl_connect('button_press_event', self.on_press)
        self.cid_release = self.fig.canvas.mpl_connect('button_release_event', self.on_release)
        self.cid_motion = self.fig.canvas.mpl_connect('motion_notify_event', self.on_motion)

    def on_press(self, event):
        if event.inaxes != self.ax: return
        for node, (x, y) in self.pos.items():
            dx, dy = x - event.xdata, y - event.ydata
            if dx*dx + dy*dy < 0.01:  # pick tolerance
                self.dragging_node = node
                break

    def on_release(self, event):
        self.dragging_node = None
        self.redraw()

    def on_motion(self, event):
        if self.dragging_node is None or event.inaxes != self.ax: return
        self.pos[self.dragging_node] = (event.xdata, event.ydata)
        self.redraw()

    def redraw(self):
        self.ax.clear()
        nx.draw_networkx_nodes(self.G, self.pos, ax=self.ax, node_color="skyblue", node_size=500)
        nx.draw_networkx_labels(self.G, self.pos, ax=self.ax)
        if self.weighted:
            nx.draw_networkx_edges(self.G, self.pos, ax=self.ax)
            labels = {(u,v): f"{self.G[u][v]['weight']}" for u,v in self.G.edges()}
            nx.draw_networkx_edge_labels(self.G, self.pos, edge_labels=labels, ax=self.ax)
        else:
            nx.draw_networkx_edges(self.G, self.pos, ax=self.ax)
        self.fig.canvas.draw()

    def show(self):
        self.fig, self.ax = plt.subplots(figsize=(7, 5))
        self.redraw()
        self.connect()
        plt.show()


# Example usage:
edges = [
    (0, 1, 706),
    (0, 4, 229),
    (0, 6, 128),
    (0, 12, 151),
    (0, 14, 985),
    (0, 18, 659),
    (0, 22, 921),
    (0, 23, 225),
    (0, 26, 423),
    (0, 29, 270),
    (1, 3, 397),
    (1, 5, 82),
    (1, 10, 631),
    (1, 12, 85),
    (1, 15, 293),
    (1, 19, 973),
    (1, 20, 673),
    (1, 21, 851),
    (1, 24, 626),
    (1, 25, 386),
    (1, 26, 223),
    (2, 3, 300),
    (2, 9, 641),
    (2, 17, 43),
    (2, 18, 899),
    (2, 29, 714),
    (3, 9, 299),
    (3, 10, 191),
    (3, 11, 525),
    (3, 14, 591),
    (3, 15, 210),
    (3, 16, 582),
    (3, 17, 820),
    (3, 20, 337),
    (3, 21, 733),
    (3, 27, 156),
    (3, 28, 995),
    (3, 29, 5),
    (4, 6, 380),
    (4, 14, 770),
    (4, 15, 274),
    (4, 24, 777),
    (4, 26, 851),
    (4, 28, 256),
    (4, 29, 861),
    (5, 6, 143),
    (5, 10, 580),
    (5, 12, 885),
    (5, 17, 994),
    (5, 21, 206),
    (5, 22, 622),
    (5, 26, 568),
    (5, 28, 505),
    (5, 29, 614),
    (6, 7, 962),
    (6, 8, 755),
    (6, 12, 327),
    (6, 15, 260),
    (6, 16, 945),
    (6, 17, 203),
    (6, 21, 203),
    (6, 23, 507),
    (7, 9, 785),
    (7, 18, 22),
    (7, 21, 843),
    (7, 26, 869),
    (7, 27, 529),
    (7, 29, 190),
    (8, 11, 873),
    (8, 12, 909),
    (8, 14, 959),
    (8, 16, 499),
    (8, 23, 37),
    (8, 24, 809),
    (8, 25, 754),
    (9, 10, 249),
    (9, 16, 304),
    (9, 18, 334),
    (9, 27, 134),
    (10, 11, 649),
    (10, 13, 891),
    (10, 14, 755),
    (10, 15, 568),
    (10, 21, 747),
    (10, 22, 369),
    (10, 24, 530),
    (10, 26, 501),
    (11, 19, 47),
    (11, 21, 789),
    (11, 22, 798),
    (11, 27, 250),
    (12, 13, 991),
    (12, 14, 304),
    (12, 15, 34),
    (12, 17, 364),
    (12, 27, 498),
    (12, 29, 254),
    (13, 14, 893),
    (13, 15, 687),
    (13, 16, 126),
    (13, 17, 153),
    (13, 18, 997),
    (14, 16, 976),
    (14, 19, 189),
    (14, 21, 158),
    (14, 27, 730),
    (14, 28, 437),
    (14, 29, 461),
    (15, 24, 415),
    (15, 29, 922),
    (16, 20, 461),
    (16, 23, 305),
    (16, 27, 29),
    (16, 29, 28),
    (17, 20, 51),
    (17, 21, 749),
    (17, 25, 557),
    (17, 26, 903),
    (17, 28, 795),
    (18, 19, 698),
    (18, 20, 700),
    (18, 23, 44),
    (18, 26, 40),
    (19, 22, 3),
    (19, 29, 429),
    (20, 25, 404),
    (20, 26, 501),
    (20, 29, 682),
    (21, 26, 648),
    (21, 28, 539),
    (22, 26, 160),
    (22, 28, 152),
    (24, 29, 536),
    (25, 26, 135),
    (28, 29, 340),
]
G = nx.Graph()
for u,v,w in edges:
    G.add_edge(u,v,weight=w)

dg = DraggableGraph(G, weighted=True)
dg.show()

