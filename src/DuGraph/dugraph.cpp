#include "dugraph.h"

#include <QQueue>

DuGraph::~DuGraph()
{
    qDeleteAll(_edges);
    qDeleteAll(_vertices);
}

QSet<DuGraph::Vertex*> DuGraph::adjacent(Vertex *vertex) const
{
    QSet<DuGraph::Vertex*> adjacent;
    for(Edge *edge: qAsConst(vertex->edges)) {
        Vertex *other = edge->to;
        if (other == vertex) other = edge->from;
        if (_type != Undirected && _type != Directed && other == vertex) continue;
        adjacent << vertex;
    }
    return adjacent;
}

QSet<DuGraph::Vertex *> DuGraph::roots() const
{
    if (_type == Undirected) return QSet<Vertex*>();
    QSet<Vertex*> r;
    for(Vertex *vertex: qAsConst(_vertices))
    {
        if (vertex->inputs.isEmpty()) r << vertex;
    }
    return r;
}

QSet<DuGraph::Vertex *> DuGraph::branches() const
{
    if (_type == Undirected) return  QSet<Vertex*>();
    QSet<Vertex*> b;
    for(Vertex *vertex: qAsConst(_vertices))
    {
        if (!vertex->inputs.isEmpty() && !vertex->edges.isEmpty())
            b << vertex;
    }
    return b;
}

QSet<DuGraph::Vertex *> DuGraph::tips() const
{
    if (_type == Undirected) return  QSet<Vertex*>();
    QSet<Vertex*> t;
    for(Vertex *vertex: qAsConst(_vertices))
    {
        if (!vertex->edges.isEmpty())
            t << vertex;
    }
    return t;
}

QSet<DuGraph::Vertex *> DuGraph::orphans() const
{
    if (_type == Undirected) return  QSet<Vertex*>();
    QSet<Vertex*> o;
    for(Vertex *vertex: qAsConst(_vertices))
    {
        if (vertex->inputs.isEmpty() && vertex->edges.isEmpty())
            o << vertex;
    }
    return o;
}

QVector<DuGraph::Vertex *> DuGraph::pathsTo(Vertex *vertex) const
{
    QVector<Vertex*> paths;
    paths << vertex;

    // BFS Algorithm
    QQueue<Vertex*> queue;
    queue.enqueue(vertex);
    while(!queue.isEmpty()) {
        Vertex *v = queue.dequeue();
        for(Edge *edge: qAsConst(v->inputs)) {
            Vertex *from = edge->from;
            if (paths.contains(from)) continue;
            paths.prepend(from);
            queue.enqueue(from);
        }
    }

    return paths;
}

bool DuGraph::validate()
{
    switch(_type) {
    case Undirected:
    case Directed:
        _isValid = true;
        return true;
    case SimpleDirected:
        _isValid = validateSDG();
        return _isValid;
    case Oriented:
        _isValid = validateOG();
        return _isValid;
    case DirectedAcyclic:
        _isValid = validateDAG();
        return _isValid;
    case MultiTree:
        _isValid = validateMTG();
        return _isValid;
    case RootedTree:
        _isValid = validateRTG();
        return _isValid;
    }
}

bool DuGraph::hasPath(Vertex *from, Vertex *to, QSet<Edge> *visited) const
{
    for (Edge *edge: qAsConst(from->edges)) {
        // Don't follow twice the same edge
        if (visited->contains(*edge)) continue;
        visited->insert(*edge);

        Q_ASSERT(edge->from);
        Q_ASSERT(edge->to);

        Vertex *other = edge->to;
        if (other == from) other = edge->from;

        if (other == from) continue;
        if (other == to) return true;
        if (hasPath(edge->to, to, visited)) return true;
    }
    return false;
}

DuGraph::Vertex *DuGraph::addVertex(const QVariant &data, const QPoint &position, double weight)
{
    auto v = new Vertex();
    v->data = data;
    v->position = position;
    v->weight = weight;
    v->id = _id++;
    _vertices << v;
    return v;
}

void DuGraph::removeVertex(Vertex *vertex)
{
    // Remove connected edges
    QSet<Edge*> removed;
    for(Edge *edge: qAsConst(_edges)) {
        if (edge->to == vertex && !removed.contains(edge)) removed << edge;
        else if (edge->from == vertex && !removed.contains(edge)) removed << edge;
    }
    for(Edge *edge: qAsConst(removed)) {
        removeEdge(edge);
    }

    // Remove the vertex
    _vertices.removeAll(vertex);
    delete vertex;
}

DuGraph::Edge *DuGraph::addEdge(Vertex *from, Vertex *to, const QVariant &data, int weight)
{
    Q_ASSERT(from);
    Q_ASSERT(to);

    if (_type >= SimpleDirected && (from == to)) throw std::invalid_argument("In simple directed graphs, edges' 'from' and 'to' can't be the same vertex");

    auto e = new Edge();
    e->from = from;
    e->to = to;
    e->data = data;
    e->weight = weight;
    e->id = _id++;

    from->edges << e;

    if (_type == Undirected) to->edges << e;
    else to->inputs << e;

    _edges << e;

    return e;
}

void DuGraph::removeEdge(Edge *edge)
{
    Vertex *to = edge->to;
    to->edges.removeAll(edge);
    Vertex *from = edge->from;
    from->edges.removeAll(edge);
    _edges.remove(edge);
    delete edge;
}

void DuGraph::order(SearchSortAlgorithm algorithm, Qt::SortOrder sortOrder)
{
    if (_type < SimpleDirected) return;

    QElapsedTimer t;
    t.start();

    // Sort
    switch(algorithm) {
    case BreadthFirst:
        qDebug() << "Ordering graph with Breadth-First algorithm";
        breadthFirstSort(sortOrder);
        break;
    case DepthFirst:
        qDebug() << "Ordering graph with Depth-First algorithm";
        depthFirstSort(sortOrder);
        break;
    case Kahn:
        qDebug() << "Ordering graph with Kahn algorithm";
        kahnSort(sortOrder);
        break;
    }

    // Result
    _vertices.swap(_orderingCache);

    // Release cache
    _orderingCache.clear();
    _visited.clear();

    qDebug().noquote() << "Graph ordered in " << t.elapsed() << " ms.";
}

void DuGraph::layout()
{
    if (_type < SimpleDirected) return;
    // First, order
    order();

    // Find X (the layer) for each vertex
    // And count how many there are (Y) on each layer
    QHash<int,int> layerCount;
    for(auto v: qAsConst(_vertices)) {
        int layer = 0;
        for (auto e: qAsConst(v->inputs)) {
            auto from = e->from;
            layer = std::max( from->position.x()+1, layer);
        }
        int count = layerCount.value(layer) + 1;
        layerCount.insert(layer, count);
        if (count % 2 == 0) count = count / 2;
        else count = -count/2;
        v->position = QPoint(layer, count);
    }
}

bool DuGraph::validateSDG()
{
    QSet<Edge> visited;
    for(Edge *edge: qAsConst(_edges))
    {
        // No self vertex connection
        if (edge->to == edge->from) edge->isValid = false;
        // Single edge between vertices
        else if (visited.contains(*edge)) edge->isValid = false;

        if (!edge->isValid) return false;
        visited << *edge;
    }
    return true;
}

bool DuGraph::validateOG()
{
    QSet<Edge> visited;
    for(Edge *edge: qAsConst(_edges))
    {
        if (edge->to == edge->from) edge->isValid = false;
        else if (visited.contains(*edge)) edge->isValid = false;
        else {
            // No 2-vertex cycle
            Q_ASSERT(edge->to);
            Q_ASSERT(edge->from);
            for(Edge *nextEdge: qAsConst(edge->to->edges) )
            {
                if (nextEdge->to == edge->from) return false;
            }
        }

        if (!edge->isValid) return false;
        visited << *edge;
    }
    return true;
}

bool DuGraph::validateDAG()
{
    QSet<Edge> visited;
    for(Edge *edge: qAsConst(_edges))
    {
        if (edge->to == edge->from) edge->isValid = false;
        else if (visited.contains(*edge)) edge->isValid = false;
        // No cycle
        else if (hasPath(edge->from, edge->from)) edge->isValid = false;

        if (!edge->isValid) return false;
        visited << *edge;
    }
    return true;
}

bool DuGraph::validateMTG()
{
    QSet<Edge> visited;

    for(Edge *edge: qAsConst(_edges))
    {
        if (edge->to == edge->from) edge->isValid = false;
        else if (visited.contains(*edge)) edge->isValid = false;
        else if (hasPath(edge->from, edge->from)) edge->isValid = false;

        if (!edge->isValid) return false;
        visited << *edge;
    }

    // A Single path between two given nodes
    for(Vertex *vertex: qAsConst(_vertices)) {
        QSet<Vertex*> path;
        if (!walkMTG(vertex, &path)) return false;
    }

    return true;
}

bool DuGraph::walkMTG(Vertex *vertex, QSet<Vertex *> *path)
{
    for(Edge *edge: qAsConst(vertex->edges)) {
        if (path->contains(edge->to)) return false;
        path->insert(edge->to);
        if (!walkMTG(edge->to, path)) return false;
    }
    return true;
}

bool DuGraph::validateRTG()
{
    QSet<Edge> visited;
    QSet<Vertex*> destinations;


    bool found = false;
    for(Vertex *vertex: qAsConst(_vertices)) {
        // A Single vertex without inputs
        if(vertex->inputs.isEmpty()) {
            if (found) return false;
            found = true;
        }
        // A Single path between two given nodes
        QSet<Vertex*> path;
        if (!walkMTG(vertex, &path)) return false;
    }

    for(Edge *edge: qAsConst(_edges))
    {
        if (edge->to == edge->from) edge->isValid = false;
        else if (visited.contains(*edge)) edge->isValid = false;
        // No cycle
        else if (hasPath(edge->from, edge->from)) edge->isValid = false;

        if (!edge->isValid) return false;
        visited << *edge;
    }
    return true;
}

void DuGraph::kahnSort(Qt::SortOrder sortOrder)
{
    QSet<Vertex*> r = roots();
    QSet<Edge*> visitedEdges;

    while(!r.isEmpty()) {

        Vertex *root = *r.constBegin();
        r.remove(root);

        if (sortOrder == Qt::AscendingOrder) _orderingCache.append(root);
        else _orderingCache.prepend(root);

        for (Edge *edge: qAsConst(root->edges)) {
            if (visitedEdges.contains(edge)) continue; // prevent infinite loops with graph cycles
            visitedEdges << edge;

            Vertex *v = edge->to;
            bool ok = true;
            for (Edge *incoming: qAsConst(v->inputs)) {
                if (!visitedEdges.contains(incoming)) {
                    ok = false;
                    break;
                }
            }
            if (ok) r << v;
        }
    }
}

void DuGraph::depthFirstSort(Qt::SortOrder sortOrder)
{
    int total = _vertices.count();

    while (_orderingCache.count() < total) {
        Vertex *vertex = _vertices.constFirst();
        depthFirstSort(vertex, sortOrder);
    }
}

void DuGraph::depthFirstSort(Vertex *vertex, Qt::SortOrder sortOrder)
{
    if (_orderingCache.contains(vertex)) return;
    if (_visited.contains(vertex)) {
        qWarning() << "Sorting a non-DAG graph (a graph with cycles).";
        return;
    }
    _visited << vertex;

    for(Edge *edge: qAsConst(vertex->edges)) {
        depthFirstSort(edge->to, sortOrder);
    }

    _visited.remove(vertex);
    if (sortOrder == Qt::AscendingOrder) _orderingCache.prepend(vertex);
    else _orderingCache.append(vertex);
}

void DuGraph::breadthFirstSort(Qt::SortOrder sortOrder)
{
    const QSet<Vertex*> r = roots();
    for(Vertex *root: r) _orderingCache << root;
    for(Vertex *root: r) breadthFirstSort(root, sortOrder);
}

void DuGraph::breadthFirstSort(Vertex *root, Qt::SortOrder sortOrder)
{
    QQueue<Vertex*> queue;
    queue.enqueue(root);

    while(!queue.isEmpty()) {
        Vertex *vertex = queue.dequeue();
        for (Edge *edge: qAsConst(vertex->edges)) {
            Vertex *to = edge->to;
            if (_visited.contains(to)) continue;
            _visited << to;
            if (sortOrder == Qt::AscendingOrder) _orderingCache.append(to);
            else if (sortOrder == Qt::DescendingOrder) _orderingCache.prepend(to);
            else _orderingCache.prepend(to);
            queue.enqueue(to);
        }
    }
}
