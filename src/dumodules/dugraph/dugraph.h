#ifndef DUGRAPH_H
#define DUGRAPH_H

#include "qpoint.h"
#include "qvariant.h"
#include <QSet>

class DuGraph
{
public:

    enum SearchSortAlgorithm {
        BreadthFirst,
        DepthFirst,
        Kahn
    };

    enum GraphType {
        /**
         * The most simple graph, without constraint
         */
        Undirected = 0,
        /**
         * A graph with oriented edges
         */
        Directed = 1,
        /**
         * A directed graph with no edges going from and to the same vertex (x,x) edges,
         * and with at most two edges (one for each orientation) between two different vertices.
         */
        SimpleDirected = 2,
        /**
         * A simple directed graph, without loops (i.e. without any pair of edges (x,y) and (y,x))
         */
        Oriented = 3,
        /**
         * A DAG, an oriented graph without cycles
         */
        DirectedAcyclic = 4,
        /**
         * A DAG with at most one path between two vertices
         */
        MultiTree = 5,
        /**
         * A Tree with at most one vertex without inputs (root)
         */
        RootedTree = 6
    };

    struct Vertex;
    struct Edge {
        Vertex *from;
        Vertex *to;
        double weight = 1;
        QVariant data;
        bool isValid = true;
        int id = -1;
    };

    struct Vertex {
        QVector<Edge*> edges = QVector<Edge*>();
        QVector<Edge*> inputs = QVector<Edge*>();
        double weight = 1;
        QVariant data = QVariant();
        QPoint position;
        bool isValid = true;
        int id = -1;
    };

    DuGraph( GraphType type = Undirected ):
        _type(type),
        _isValid(true)
        {};
    DuGraph(DuGraph const&) = default;
    ~DuGraph();

    // Properties
    bool isValid() const { return _isValid; };
    bool type() const { return _type; };
    QVector<Vertex*> vertices() const { return _vertices; };
    QSet<Edge*> edges() const { return _edges; };

    // Navigation
    /**
     * @brief adjacent Gets the directly connected vertices
     * @param vertex The origin
     * @return The vertices connect to the vertex
     */
    QSet<Vertex*> adjacent(Vertex *vertex) const;
    /**
     * @brief roots In a directed graph, gets all vertices without incoming edges
     * @return An empty list if the graph is undirected
     */
    QSet<Vertex*> roots() const;
    /**
     * @brief branches In a directed graph, gets all vertices with both outgoing and incoming edges
     * @return An empty list if the graph is undirected
     */
    QSet<Vertex*> branches() const;
    /**
     * @brief tips In a directed graph, gets all vertices without outgoing edges
     * @return An empty list if the graph is undirected
     */
    QSet<Vertex*> tips() const;
    /**
     * @brief orphans Gets all vertices without outgoing nor incoming edges
     * @return
     */
    QSet<Vertex*> orphans() const;
    /**
     * @brief pathsTo Gets all vertices leading to this one
     * @param vertex
     * @return
     */
    QVector<Vertex*> pathsTo(Vertex *vertex) const;

    // Validation
    bool validate();
    bool hasPath(Vertex *from, Vertex *to, QSet<Edge> *visited = new QSet<Edge>()) const;

    // Creation
    Vertex *addVertex(const QVariant &data = QVariant(), const QPoint &position = QPoint(), double weight = 1);
    void removeVertex(Vertex *vertex);
    Edge *addEdge(Vertex *from, Vertex *to, const QVariant &data = QVariant(), int weight = 1);
    void removeEdge(Edge *edge);

    // Order and Layout
    // For now, implemented for >= SimpleDirected only
    void order(SearchSortAlgorithm algorithm = Kahn, Qt::SortOrder sortOrder = Qt::AscendingOrder);
    void layout();

private:
    // Data
    QVector<Vertex*> _vertices;
    QVector<Vertex*> _orderingCache;
    QSet<Vertex*> _visited;
    QSet<Edge*> _edges;
    GraphType _type;
    bool _isValid;
    int _id = 0;

    // Validation
    bool validateSDG();
    bool validateOG();
    bool validateDAG();
    bool validateMTG();
    bool walkMTG(Vertex *vertex, QSet<Vertex*> *path);
    bool validateRTG();

    // Ordering
    void kahnSort(Qt::SortOrder sortOrder);
    void depthFirstSort(Qt::SortOrder sortOrder);
    void depthFirstSort(Vertex *vertex, Qt::SortOrder sortOrder);
    void breadthFirstSort(Qt::SortOrder sortOrder);
    void breadthFirstSort(Vertex *root, Qt::SortOrder sortOrder);


#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    template <typename T, template<typename> class C>
    static QSet<T> toSet(const C<T> &container)
    {
        return QSet<T>(container.begin(), container.end());
    }
#else
    template <typename T, template<typename> class C>
    static QSet<T> toSet(const C<T> &container)
    {
        QSet<T> set;
        for(auto v: container) {
            set << v;
        }
        return set;
    }
#endif
};

inline bool operator==(const DuGraph::Edge &a, const DuGraph::Edge &b) {
    return a.to == b.to && a.from == b.from;
}

inline uint qHash(const DuGraph::Edge &edge, uint seed = 0) noexcept {
    return qHash(QString::number(edge.from->id) + QString::number(edge.to->id), seed);
}

#endif // DUGRAPH_H
