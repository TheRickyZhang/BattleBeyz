# dependency_graph_generation.py
# Run via:
#   pip install -r py_requirements.txt
#   python dependency_graph_generation.py
# Then convert to PNG:
#   dot -Tpng dependencies.dot -Gdpi=300 -o Battlebeyz_dependencies.png

import os
import re
from clang.cindex import Config
import pygraphviz as pgv

# Adjust path to libclang.dll if needed
Config.set_library_file(r"C:\Program Files\LLVM\bin\libclang.dll")

# These are relative to here, which should be Battlebeyz/meta
SRC_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "../src"))
OUTPUT_GRAPH = os.path.join(os.path.dirname(__file__), "dependencies.dot")

# Logical grouping of headers into clusters (use only folder names)
logical_groups = {
    "Camera":     "Camera",
    "Config":     "Config",
    "GameObjects":"GameObjects",
    "MeshObjects":"MeshObjects",
    "Physics":    "Physics",
    "Rendering":  "Rendering",
    "RigidBodies":"RigidBodies",
    "States":     "States",
    "UI":         "UI",
}

# Dependency graph
dependency_graph = {}
main_includes = set()  # Store files included by main.cpp


def get_files_by_folder(groups, src_dir):
    """
    Automatically gather .h files for each folder in logical_groups.
    """
    grouped_files = {key: [] for key in groups.keys()}
    for root, _, files in os.walk(src_dir):
        for file in files:
            if file.endswith(".h"):
                rel_path = os.path.relpath(os.path.join(root, file), src_dir).replace("\\", "/")
                for group_name, folder in groups.items():
                    if rel_path.startswith(folder + "/"):
                        grouped_files[group_name].append(rel_path)
                        break
    return grouped_files


def parse_file(file_path):
    """
    Parse a file and collect all #include dependencies that exist within SRC_DIR.
    Ensures every included file is normalized to the same canonical path.
    """
    includes = set()
    try:
        with open(file_path, "r") as f:
            for line in f:
                match = re.match(r'#include\s+"(.+)"', line)
                if match:
                    inc = match.group(1).replace("\\", "/")

                    # 1) Try building from SRC_DIR so "ActiveState.h"
                    #    becomes "States/ActiveState.h" if it physically lives there.
                    candidate_abs = os.path.abspath(os.path.join(SRC_DIR, inc))

                    # 2) If that exact path doesn't exist, fallback to local dir.
                    if not os.path.exists(candidate_abs):
                        candidate_abs = os.path.abspath(
                            os.path.join(os.path.dirname(file_path), inc)
                        )

                    # 3) If still under SRC_DIR, convert to rel path
                    if candidate_abs.startswith(SRC_DIR):
                        rel_path = os.path.relpath(candidate_abs, SRC_DIR).replace("\\", "/")
                        includes.add(rel_path)

    except Exception as e:
        print(f"Error reading {file_path}: {e}")
    return includes

def build_graph():
    """
    Build the dependency graph by parsing all header files in SRC_DIR
    and store files included by main.cpp separately.
    """
    global main_includes
    for root, _, files in os.walk(SRC_DIR):
        for file in files:
            file_path = os.path.join(root, file)
            rel_path = os.path.relpath(file_path, SRC_DIR).replace("\\", "/")

            if file.endswith(".h"):
                dependency_graph[rel_path] = parse_file(file_path)
            elif rel_path == "main.cpp":
                main_includes = parse_file(file_path)  # Store includes for main.cpp


def file_label(path):
    return path.split("/")[-1]


def topological_sort(graph):
    """
    Kahn's algorithm for topological sorting.
    """
    all_files = set(graph.keys())
    for deps in graph.values():
        all_files.update(deps)

    in_degree = {f: 0 for f in all_files}
    for node, deps in graph.items():
        for d in deps:
            in_degree[d] += 1

    queue = [n for n in in_degree if in_degree[n] == 0]
    sorted_nodes = []
    while queue:
        node = queue.pop(0)
        sorted_nodes.append(node)
        if node in graph:
            for d in graph[node]:
                in_degree[d] -= 1
                if in_degree[d] == 0:
                    queue.append(d)

    # Append any cycles or leftover
    for f in in_degree:
        if in_degree[f] > 0 and f not in sorted_nodes:
            sorted_nodes.append(f)

    return sorted_nodes


def visualize_graph():
    print("Visualizing dependency graph...")

    # Build a mapping of folder->files but don't add nodes yet
    grouped_files = get_files_by_folder(logical_groups, SRC_DIR)

    # Gather all nodes (files + dependencies)
    all_nodes = set(dependency_graph.keys())
    for deps in dependency_graph.values():
        all_nodes.update(deps)

    sorted_nodes = topological_sort(dependency_graph)

    # Initialize the main graph
    graph = pgv.AGraph(strict=True, directed=True)
    graph.graph_attr.update({
        "rankdir": "TB",  # Top-to-bottom layout
        "splines": "true",
        "size": "10,8!",
        "pack": "true",
        "packmode": "clust",
    })
    graph.node_attr.update({
        "shape": "rect",
        "fontsize": "8",
    })
    graph.edge_attr.update({
        "len": "0.3",
        "arrowsize": "0.4",
    })

    # Create subgraphs for each folder
    subgraphs = {}
    for group_name in sorted(grouped_files.keys()):
        subg = graph.subgraph(name=f"cluster_{group_name}", label=group_name)
        subg.graph_attr.update({"style": "rounded", "rankdir": "TB"})
        subgraphs[group_name] = subg

    # Keep track of added nodes
    nodes_in_subgraphs = set()

    # --- Add nodes and edges ---
    for file in sorted_nodes:
        dependencies = dependency_graph.get(file, set())

        # Add the file to the correct subgraph
        placed_in_subgraph = False
        for group_name, folder_subg in subgraphs.items():
            if file.startswith(logical_groups[group_name] + "/"):
                if file not in nodes_in_subgraphs:
                    folder_subg.add_node(file, label=file_label(file))
                    nodes_in_subgraphs.add(file)
                placed_in_subgraph = True
                break

        # If the file does not belong to any subgraph, add it to the main graph
        if not placed_in_subgraph and file not in nodes_in_subgraphs:
            graph.add_node(file, label=file_label(file))
            nodes_in_subgraphs.add(file)  # Still track it

        # Add dependencies as edges
        for dep in dependencies:
            # Add the dependency to the correct subgraph or the main graph
            dep_placed_in_subgraph = False
            for group_name, folder_subg in subgraphs.items():
                if dep.startswith(logical_groups[group_name] + "/"):
                    if dep not in nodes_in_subgraphs:
                        folder_subg.add_node(dep, label=file_label(dep))
                        nodes_in_subgraphs.add(dep)
                    dep_placed_in_subgraph = True
                    break

            # If dependency does not belong to any subgraph, add it to the main graph
            if not dep_placed_in_subgraph and dep not in nodes_in_subgraphs:
                graph.add_node(dep, label=file_label(dep))
                nodes_in_subgraphs.add(dep)

            # Add the edge
            graph.add_edge(file, dep)

    # Highlight nodes included by main.cpp (now all nodes are properly in subgraphs or main graph)
    for included_file in main_includes:
        # Highlight the node only if it exists in nodes_in_subgraphs (ensured now)
        if included_file in nodes_in_subgraphs:
            n = graph.get_node(included_file)
            n.attr["shape"] = "rect"
            n.attr["style"] = "bold, filled"
            n.attr["fillcolor"] = "cyan"

    # Place the legend explicitly in the top-left corner
    legend = graph.subgraph(name="cluster_legend", label="Legend", style="filled, rounded", color="lightgrey")
    legend.graph_attr.update({"rank": "source"})  # Ensure it stays at the top
    legend.add_node("legend_incoming", label="3+ Incoming Edges", shape="rect", fillcolor="yellow", style="filled")
    legend.add_node("legend_outgoing", label="3+ Outgoing Edges", shape="rect", fillcolor="lightgreen", style="filled")
    legend.add_node("legend_main_includes", label="Included by main.cpp", shape="rect", fillcolor="cyan", style="bold,filled")

    # Write the graph to a .dot file
    graph.write(OUTPUT_GRAPH)
    print(f"Dependency graph saved to {OUTPUT_GRAPH}")


if __name__ == "__main__":
    build_graph()
    visualize_graph()
