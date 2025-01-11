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

# Logical grouping (unused now, but left for reference)
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

dependency_graph = {}
main_includes = set()

def get_files_by_folder(groups, src_dir):
    grouped_files = {key: [] for key in groups.keys()}
    file_to_groups = {}

    for root, _, files in os.walk(src_dir):
        for file in files:
            if file.endswith(".h"):
                rel_path = os.path.relpath(os.path.join(root, file), src_dir).replace("\\", "/")
                for group_name, folder in groups.items():
                    if rel_path.startswith(folder + "/"):
                        if rel_path not in file_to_groups:
                            file_to_groups[rel_path] = set()
                        file_to_groups[rel_path].add(group_name)

                        if rel_path not in grouped_files[group_name]:
                            grouped_files[group_name].append(rel_path)

    for file, groups_set in file_to_groups.items():
        if len(groups_set) > 1:
            print(f"File '{file}' belongs to multiple groups: {', '.join(groups_set)}")

    return grouped_files

# Define all include directories from your build system
INCLUDE_DIRS = [
    os.path.abspath(os.path.join(SRC_DIR, "..", "assets")),
    SRC_DIR,
    os.path.abspath(os.path.join(SRC_DIR, "Camera")),
    os.path.abspath(os.path.join(SRC_DIR, "GameObjects")),
    os.path.abspath(os.path.join(SRC_DIR, "MeshObjects")),
    os.path.abspath(os.path.join(SRC_DIR, "mini")),
    os.path.abspath(os.path.join(SRC_DIR, "Physics")),
    os.path.abspath(os.path.join(SRC_DIR, "Physics", "Units")),
    os.path.abspath(os.path.join(SRC_DIR, "Rendering")),
    os.path.abspath(os.path.join(SRC_DIR, "RigidBodies")),
    os.path.abspath(os.path.join(SRC_DIR, "States")),
    os.path.abspath(os.path.join(SRC_DIR, "Config")),
    os.path.abspath(os.path.join(SRC_DIR, "UI")),
]

def resolve_include(include_path, current_file):
    """
    Resolve an #include path to its canonical absolute path using INCLUDE_DIRS.
    Tries each include directory in order and returns the first match.
    """
    for include_dir in INCLUDE_DIRS:
        candidate_abs = os.path.abspath(os.path.join(include_dir, include_path))
        if os.path.exists(candidate_abs):
            return os.path.normpath(candidate_abs)

    # Fall back to resolving relative to the current file's directory
    candidate_abs = os.path.abspath(os.path.join(os.path.dirname(current_file), include_path))
    if os.path.exists(candidate_abs):
        return os.path.normpath(candidate_abs)

    return None  # Return None if the file cannot be resolved

def parse_file(file_path):
    """
    Parse a file and collect all #include dependencies.
    Resolves each include using resolve_include().
    """
    includes = set()
    try:
        with open(file_path, "r") as f:
            for line in f:
                match = re.match(r'#include\s+"(.+)"', line)
                if match:
                    inc = match.group(1).replace("\\", "/")
                    resolved_path = resolve_include(inc, file_path)
                    if resolved_path:
                        includes.add(resolved_path)
    except Exception as e:
        print(f"Error reading {file_path}: {e}")
    return includes

def build_graph():
    """
    Build the dependency graph, using canonical paths for uniqueness.
    """
    global main_includes
    for root, _, files in os.walk(SRC_DIR):
        for file in files:
            file_path = os.path.abspath(os.path.join(root, file))
            if file.endswith(".h"):
                canonical_path = os.path.normpath(file_path)
                dependency_graph[canonical_path] = parse_file(file_path)
            elif os.path.basename(file_path) == "main.cpp":
                main_includes = parse_file(file_path)

# Visualization logic remains unchanged


def file_label(path):
    """
    Create a display label using only the file name.
    """
    return os.path.basename(path)

def visualize_graph():
    """
    Visualize the dependency graph with highlighting and proper rendering of no-edge nodes.
    """
    print("Visualizing dependency graph...")

    # Flatten the graph to include all unique nodes
    all_nodes = set(dependency_graph.keys())
    for deps in dependency_graph.values():
        all_nodes.update(deps)

    # Create the graph
    graph = pgv.AGraph(directed=True, strict=True)  # strict=True ensures no duplicate edges
    graph.graph_attr.update({
        "rankdir": "TB",
        "splines": "true",
    })
    graph.node_attr.update({
        "shape": "rect",
        "fontsize": "8",
    })
    graph.edge_attr.update({
        "arrowsize": "0.4",
    })

    # Add all nodes using their canonical paths, but display only the file name
    for file in all_nodes:
        graph.add_node(file, label=file_label(file))

    # Add edges for dependencies and count incoming/outgoing edges
    incoming_counts = {file: 0 for file in all_nodes}
    outgoing_counts = {file: 0 for file in all_nodes}

    for file, dependencies in dependency_graph.items():
        for dep in dependencies:
            graph.add_edge(file, dep)
            incoming_counts[dep] += 1
            outgoing_counts[file] += 1

    # Highlight nodes with 3+ incoming/outgoing edges
    for file in all_nodes:
        node = graph.get_node(file)
        if incoming_counts[file] >= 3:
            node.attr["fillcolor"] = "yellow"
            node.attr["style"] = "filled"
        if outgoing_counts[file] >= 3:
            node.attr["fillcolor"] = "lightgreen"
            node.attr["style"] = "filled"

    # Ensure all no-edge nodes are rendered at the bottom
    no_edge_nodes = [file for file in all_nodes if incoming_counts[file] == 0 and outgoing_counts[file] == 0]
    if no_edge_nodes:
        bottom_rank = graph.subgraph(name="cluster_no_edges", rank="same", label="No Edges", style="invis")
        for file in no_edge_nodes:
            bottom_rank.add_node(file)

    # Highlight nodes included by main.cpp
    for included_file in main_includes:
        if included_file in all_nodes:
            node = graph.get_node(included_file)
            node.attr["style"] = "bold, filled"
            node.attr["fillcolor"] = "cyan"

    # Write to the DOT file
    graph.write(OUTPUT_GRAPH)
    print(f"Dependency graph saved to {OUTPUT_GRAPH}")


if __name__ == "__main__":
    build_graph()
    visualize_graph()
