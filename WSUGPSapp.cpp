#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include "Button.h"
#include "Node.h"
#include "Edge.h"
#include "TextBox.h"
#include "FileSystemTextBox.h"
#include "DijkstraAlgorithm.h"

namespace fs = std::filesystem;

void connectNewNode(Node& newNode, std::vector<Node>& allNodes, std::vector<Edge>& edges, float radius) {
    for (auto& otherNode : allNodes) {
        if (newNode.id == otherNode.id) continue;

        float dx = newNode.position.x - otherNode.position.x;
        float dy = newNode.position.y - otherNode.position.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance <= radius) {
            // Pass the IDs here!
            edges.emplace_back(newNode.position, otherNode.position, newNode.id, otherNode.id);
        }
    }
}

Node* findNodeByID(int id, std::vector<Node>& nodes) {
    for (auto& n : nodes) {
        if (n.id == id) return &n;
    }
    return nullptr;
}

int main() {
    sf::RenderWindow window(sf::VideoMode({ 2000, 1500 }), "WSU Graph Project");

    // Load the Font arial
    sf::Font font;
    fs::path fontPath = fs::current_path() / "Assets" / "arial.ttf"; // Or "arial.ttf"

    // Open the file with Standard C++
    std::ifstream fontFile(fontPath, std::ios::binary | std::ios::ate);
    if (!fontFile.is_open()) {
        std::cerr << "CRITICAL: Could not find font at " << fs::absolute(fontPath) << std::endl;
        return -1;
    }

    // Read into buffer
    std::streamsize fSize = fontFile.tellg();
    fontFile.seekg(0, std::ios::beg);
    std::vector<char> fontBuffer(fSize);
    fontFile.read(fontBuffer.data(), fSize);

    // Load from memory
    if (!font.openFromMemory(fontBuffer.data(), fontBuffer.size())) {
        std::cerr << "SFML failed to parse font data from memory" << std::endl;
        return -1;
    }

    // Load the Campus Map
    sf::Texture campusTexture;
    fs::path mapPath = fs::current_path() / "Assets" / "WSU-Campus-Map-2024.png";
    std::ifstream file(mapPath, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<char> buffer(size);
        file.read(buffer.data(), size);
        if (!campusTexture.loadFromMemory(buffer.data(), buffer.size())) {
            std::cerr << "Failed to load campus texture from memory!" << std::endl;
        }
    }
    sf::Sprite background(campusTexture);
    
    // Declaring the 7 buttons
    std::unique_ptr<Button> btnAddNode;
    std::unique_ptr<Button> btnDelNode;
    std::unique_ptr<Button> btnAddEdge;
    std::unique_ptr<Button> btnDelEdge;
    std::unique_ptr<Button> btnClear;
    std::unique_ptr<Button> btnLoad;
    std::unique_ptr<Button> btnSave;

    // Initialize them one by one
    btnAddNode = std::make_unique<Button>("Add Node", sf::Vector2f(50, 1300), font);
    btnDelNode = std::make_unique<Button>("Delete Node", sf::Vector2f(250, 1300), font);
    btnAddEdge = std::make_unique<Button>("Add Edge", sf::Vector2f(450, 1300), font);
    btnDelEdge = std::make_unique<Button>("Delete Edge", sf::Vector2f(650, 1300), font);
    btnClear = std::make_unique<Button>("Clear Graph", sf::Vector2f(850, 1300), font);
    btnLoad = std::make_unique<Button>("Load Graph", sf::Vector2f(1050, 1300), font);
    btnSave = std::make_unique<Button>("Save Graph", sf::Vector2f(1250, 1300), font);

    // Creates the text box to set edge radius
    auto txtRadius = std::make_unique<TextBox>(sf::Vector2f(450, 1365), font);

    // New file boxes under Save and Load buttons
    auto txtSaveFile = std::make_unique<FileSystemTextBox>(sf::Vector2f(1250, 1365), font, "graphFile");
    auto txtLoadFile = std::make_unique<FileSystemTextBox>(sf::Vector2f(1050, 1365), font, "graphFile");

    // Text boxes showing selected Nodes data
    auto txtNodeName1 = std::make_unique<FileSystemTextBox>(sf::Vector2f(1450, 1350), font, "");
    auto txtNodeName2 = std::make_unique<FileSystemTextBox>(sf::Vector2f(1700, 1350), font, "");
    sf::Text infoText(font, "", 18);
    infoText.setFillColor(sf::Color::White);

    // Stores all the Nodes
    std::vector<Node> nodes;
    // Stores all the Edges
    std::vector<Edge> edges;
    // Stores the currently selected Nodes
    std::vector<int> selectedNodeIDs;
    // Determines if they are currently adding a Node
    bool isAddingNode = false;
    // Determines if they are currently adding a Edge
    bool isAddingEdge = false;
    // Keeps track of Node IDs
    int nextNodeID = 0;

    // Loads basic map
    fs::path graphPath = fs::current_path() / "Assets" / "WSUMapGraph.txt";
    std::ifstream inFile(graphPath);

    if (inFile.is_open()) {
        size_t nodeCount = 0;
        int maxID = -1;

        if (inFile >> nodeCount) {
            for (size_t i = 0; i < nodeCount; ++i) {
                int id; float x, y;
                // Read the numbers first
                if (!(inFile >> id >> x >> y)) break;

                // 'std::ws' eats the single space sitting between the 'y' coordinate and the name
                std::string name;
                std::getline(inFile >> std::ws, name);

                Node newNode({ x, y }, id);
                newNode.name = name;
                nodes.push_back(newNode);
                if (id > maxID) maxID = id;
            }
            nextNodeID = maxID + 1;
        }

        size_t edgeCount = 0;
        if (inFile >> edgeCount) {
            for (size_t i = 0; i < edgeCount; ++i) {
                int uID, vID;
                if (!(inFile >> uID >> vID)) break;

                sf::Vector2f p1, p2;
                bool foundU = false, foundV = false;
                for (const auto& n : nodes) {
                    if (n.id == uID) { p1 = n.position; foundU = true; }
                    if (n.id == vID) { p2 = n.position; foundV = true; }
                }

                if (foundU && foundV) {
                    edges.emplace_back(p1, p2, uID, vID);
                }
            }
        }
        inFile.close();
        std::cout << "Successfully loaded " << nodes.size() << " nodes and " << edges.size() << " edges.\n";
    }
    else {
        // If it fails, just log it and open an empty map instead of crashing
        std::cerr << "Note: Default graph not found at " << graphPath.string() << ". Starting with empty map." << std::endl;
    }

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Handle Clicks
            if (event->is<sf::Event::MouseButtonPressed>()) {
                auto mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                    bool clickedNode = false;
                    if (!isAddingNode) {
                        for (auto& node : nodes) {
                            if (node.shape.getGlobalBounds().contains(mousePos)) {
                                clickedNode = true;
                                // Selected Nodes section
                                if (node.isSelected) {
                                    node.isSelected = false;
                                    selectedNodeIDs.erase(std::remove(selectedNodeIDs.begin(), selectedNodeIDs.end(), node.id), selectedNodeIDs.end());
                                }
                                else if (selectedNodeIDs.size() < 2) {
                                    node.isSelected = true;
                                    selectedNodeIDs.push_back(node.id);
                                }

                                // UI refresh to update text boxes names
                                // Wipe both boxes first to ensure no ghost data stays behind
                                txtNodeName1->inputString = "";
                                txtNodeName1->displayValue.setString("");
                                txtNodeName2->inputString = "";
                                txtNodeName2->displayValue.setString("");

                                // Re-populate based on the new state of the objects
                                if (selectedNodeIDs.size() >= 1) {
                                    Node* n1 = findNodeByID(selectedNodeIDs[0], nodes);
                                    if (n1) {
                                        txtNodeName1->inputString = n1->name;
                                        txtNodeName1->displayValue.setString(n1->name);
                                    }
                                }
                                if (selectedNodeIDs.size() == 2) {
                                    Node* n2 = findNodeByID(selectedNodeIDs[1], nodes);
                                    if (n2) {
                                        txtNodeName2->inputString = n2->name;
                                        txtNodeName2->displayValue.setString(n2->name);
                                    }
                                }
                            }
                        }
                    }

                    if (!clickedNode) {
                        // Checks if mouse click is inside text box
                        txtRadius->isSelected = txtRadius->box.getGlobalBounds().contains(mousePos);
                        txtSaveFile->isSelected = txtSaveFile->box.getGlobalBounds().contains(mousePos);
                        txtLoadFile->isSelected = txtLoadFile->box.getGlobalBounds().contains(mousePos);
                        txtNodeName1->isSelected = txtNodeName1->box.getGlobalBounds().contains(mousePos);
                        txtNodeName2->isSelected = txtNodeName2->box.getGlobalBounds().contains(mousePos);

                        // Checks if mouse clicked on a button
                        if (btnAddNode->isClicked(mousePos)) {
                            std::cout << "Add Node Clicked\n";
                            isAddingNode = true;
                            std::cout << "User Is Placing Node\n";
                        }
                        else if (isAddingNode) {
                            nodes.emplace_back(mousePos, nextNodeID++);

                            // Automatically connect based on the radius in the TextBox
                            connectNewNode(nodes.back(), nodes, edges, txtRadius->getValue());

                            isAddingNode = false; // puts the node down on the map
                            std::cout << "Node Added at: " << mousePos.x << ", " << mousePos.y << "\n";
                        }
                        if (btnDelNode->isClicked(mousePos) && btnDelNode->isActive) {
                            for (int selectedID : selectedNodeIDs) {
                                sf::Vector2f targetPos;
                                bool found = false;

                                // Find the position of the node we are about to delete
                                for (auto& n : nodes) {
                                    if (n.id == selectedID) {
                                        targetPos = n.position;
                                        found = true;
                                        break;
                                    }
                                }

                                if (found) {
                                    // Remove edges connected to this position
                                    edges.erase(std::remove_if(edges.begin(), edges.end(),
                                        [targetPos](const Edge& e) {
                                            return (e.line[0].position == targetPos || e.line[1].position == targetPos);
                                        }), edges.end());

                                    // Remove the node itself
                                    nodes.erase(std::remove_if(nodes.begin(), nodes.end(),
                                        [selectedID](const Node& n) { return n.id == selectedID; }),
                                        nodes.end());
                                }
                            }
                            selectedNodeIDs.clear();
                        }
                        if (btnAddEdge->isClicked(mousePos) && btnAddEdge->isActive) {
                            std::cout << "Add Edge Clicked\n";

                            // We know selectedNodeIDs.size() == 2 because of the isActive check
                            int id1 = selectedNodeIDs[0];
                            int id2 = selectedNodeIDs[1];

                            Node* node1 = nullptr;
                            Node* node2 = nullptr;

                            // Find the actual node objects to get their positions
                            for (auto& node : nodes) {
                                if (node.id == id1) {
                                    node1 = &node;
                                }
                                if (node.id == id2) {
                                    node2 = &node;
                                }
                            }

                            if (node1 && node2) {
                                // Add a new edge between these two positions
                                edges.emplace_back(node1->position, node2->position, id1, id2);
                                std::cout << "Manual Edge added between Node " << id1 << " and " << id2 << "\n";
                            }
                        }
                        if (btnDelEdge->isClicked(mousePos) && btnDelEdge->isActive) {
                            std::cout << "Delete Edge Clicked\n";

                            // We know selectedNodeIDs.size() == 2 because of the isActive check
                            int id1 = selectedNodeIDs[0];
                            int id2 = selectedNodeIDs[1];

                            sf::Vector2f pos1, pos2;
                            bool found1 = false, found2 = false;

                            // Find the actual node objects to get their positions
                            for (auto& n : nodes) {
                                if (n.id == id1) { pos1 = n.position; found1 = true; }
                                if (n.id == id2) { pos2 = n.position; found2 = true; }
                            }

                            if (found1 && found2) {
                                edges.erase(std::remove_if(edges.begin(), edges.end(),
                                    [pos1, pos2](const Edge& e) {
                                        // Check if the edge connects pos1 and pos2 (either direction)
                                        sf::Vector2f start = e.line[0].position;
                                        sf::Vector2f end = e.line[1].position;
                                        return (start == pos1 && end == pos2) || (start == pos2 && end == pos1);
                                    }), edges.end());

                                std::cout << "Edge removed between Node " << id1 << " and " << id2 << "\n";
                            }
                        }
                        if (btnClear->isClicked(mousePos)) {
                            std::cout << "Clear All Clicked\n";

                            // Clear the data vectors
                            nodes.clear();
                            edges.clear();

                            // Clear the selection state
                            selectedNodeIDs.clear();
                            txtNodeName1->inputString = "";
                            txtNodeName1->displayValue.setString("");
                            txtNodeName2->inputString = "";
                            txtNodeName2->displayValue.setString("");

                            // This ensures new nodes start back at ID 0
                            nextNodeID = 0;

                            // Reset placement states
                            isAddingNode = false;
                            isAddingEdge = false;

                            std::cout << "Graph fully reset.\n";
                        }
                        if (btnLoad->isClicked(mousePos) && btnLoad->isActive) {
                            std::string fileName = txtLoadFile->getStringValue();

                            if (fileName.empty()) {
                                fileName = "graphFile";
                            }

                            std::ifstream inFile(fileName + ".txt");

                            if (inFile.is_open()) {
                                nodes.clear();
                                edges.clear();
                                selectedNodeIDs.clear();

                                size_t nodeCount, edgeCount;
                                int maxID = -1;

                                // Load Nodes
                                if (!(inFile >> nodeCount)) {
                                    return 1;
                                }
                                for (size_t i = 0; i < nodeCount; ++i) {
                                    int id; float x, y; std::string name;
                                    inFile >> id >> x >> y >> name;

                                    Node newNode({ x, y }, id);
                                    newNode.name = name;
                                    nodes.push_back(newNode);

                                    if (id > maxID) maxID = id;
                                }
                                // Sync our global counter so next added node is unique
                                nextNodeID = maxID + 1;

                                // Load Edges
                                if (!(inFile >> edgeCount)) {
                                    return 1;
                                }
                                for (size_t i = 0; i < edgeCount; ++i) {
                                    int uID, vID;
                                    inFile >> uID >> vID;

                                    // Find the positions for these IDs to recreate the visual edge
                                    sf::Vector2f p1, p2;
                                    bool foundU = false, foundV = false;
                                    for (const auto& n : nodes) {
                                        if (n.id == uID) { p1 = n.position; foundU = true; }
                                        if (n.id == vID) { p2 = n.position; foundV = true; }
                                    }

                                    if (foundU && foundV) {
                                        edges.emplace_back(p1, p2, uID, vID);
                                    }
                                }

                                inFile.close();
                                std::cout << "Graph loaded. Next ID will be: " << nextNodeID << "\n";
                            }
                            else {
                                std::cout << "Error: The File " << nextNodeID << ".txt could not be found\n";
                            }
                        }
                        if (btnSave->isClicked(mousePos) && btnSave->isActive) {
                            std::string fileName = txtSaveFile->getStringValue();

                            if (fileName.empty()) {
                                fileName = "graphFile";
                            }

                            std::ofstream outFile(fileName + ".txt");
                            if (outFile.is_open()) {
                                // Save Nodes: Count, then [ID X Y Name]
                                outFile << nodes.size() << "\n";
                                for (const auto& n : nodes) {
                                    // Use a delimiter or ensure the name doesn't have spaces for simple parsing
                                    outFile << n.id << " " << n.position.x << " " << n.position.y << " " << n.name << "\n";
                                }

                                // Save Edges: Count, then [ID_U ID_V]
                                outFile << edges.size() << "\n";
                                for (const auto& e : edges) {
                                    outFile << e.u << " " << e.v << "\n";
                                }

                                outFile.close();
                                std::cout << "Graph successfully saved to " << fileName << ".txt\n";
                            }
                            else {
                                std::cerr << "Error: Could not open file for saving.\n";
                            }
                        }
                    }
                }
            }

            // Handle Text Box
            if (event->is<sf::Event::TextEntered>()) {
                auto textEvent = *event->getIf<sf::Event::TextEntered>();
                txtRadius->handleInput(textEvent);
                txtSaveFile->handleInput(textEvent);
                txtLoadFile->handleInput(textEvent);
                txtNodeName1->handleInput(textEvent);
                txtNodeName2->handleInput(textEvent);
                // recalculateEdges(nodes, edges, txtRadius->getValue());
            }
        }

        // Update Button States every frame
        btnAddNode->isActive = true;  // Always active
        btnClear->isActive = true;  // Always active
        btnLoad->isActive = true;  // Always active
        btnSave->isActive = true;  // Always active

        // Delete Node active if 1 or 2 nodes selected
        btnDelNode->isActive = (selectedNodeIDs.size() >= 1);

        // Edge buttons only active if exactly 2 nodes selected
        btnAddEdge->isActive = (selectedNodeIDs.size() == 2);
        btnDelEdge->isActive = (selectedNodeIDs.size() == 2);

        window.clear();

        // Draw background
        window.draw(background);
        // Finds the shortest path between nodes and stores it in Path Result
        PathResult currentPath;
        if (selectedNodeIDs.size() == 2) {
            currentPath = findShortestPath(selectedNodeIDs[0], selectedNodeIDs[1], nodes, edges);
        }
        // Draw Edges
        for (auto& edge : edges) {
            edge.draw(window);
        }
        // Draw Highlighted shortest path
        if (selectedNodeIDs.size() == 2 && currentPath.distance >= 0) {
            sf::VertexArray highlightedLines(sf::PrimitiveType::LineStrip);

            for (int pathNodeID : currentPath.path) {
                Node* n = findNodeByID(pathNodeID, nodes);
                if (n) {
                    // Add each node's position to the line and color it green
                    highlightedLines.append(sf::Vertex{ n->position, sf::Color::Green });
                }
            }
            window.draw(highlightedLines);
        }
        // Draw Nodes
        for (auto& node : nodes) {
            node.draw(window);
        }
        // Draw Node that follows mouse
        if (isAddingNode) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            Node ghostNode(mousePos, -1);

            // Set to semi-transparent red
            ghostNode.shape.setFillColor(sf::Color(255, 0, 0, 127));
            ghostNode.draw(window);
        }
        // Draw buttons
        btnAddNode->draw(window);
        btnDelNode->draw(window);
        btnAddEdge->draw(window);
        btnDelEdge->draw(window);
        btnClear->draw(window);
        btnLoad->draw(window);
        btnSave->draw(window);
        // Draw text box
        txtRadius->draw(window);
        txtSaveFile->draw(window);
        txtLoadFile->draw(window);
        // Draw selected nodes section
        if (!selectedNodeIDs.empty()) {
            Node* n1 = findNodeByID(selectedNodeIDs[0], nodes);
            if (n1) {
                // Drawing the elements
                infoText.setString("Selected Node 1:\nID: " + std::to_string(n1->id));
                infoText.setPosition({ 1450, 1300 });
                window.draw(infoText);
                txtNodeName1->draw(window);

                // Syncing system only update the node name if the box is being interacted with
                if (txtNodeName1->isSelected) {
                    n1->name = txtNodeName1->getStringValue();
                }
            }

            if (selectedNodeIDs.size() == 2) {
                Node* n2 = findNodeByID(selectedNodeIDs[1], nodes);
                if (n2) {
                    infoText.setString("Selected Node 2:\nID: " + std::to_string(n2->id));
                    infoText.setPosition({ 1700, 1300 });
                    window.draw(infoText);
                    txtNodeName2->draw(window);

                    if (txtNodeName2->isSelected) {
                        n2->name = txtNodeName2->getStringValue();
                    }

                    // Shortest Path Calculated by Dijkstra's algorithm
                    if (currentPath.distance >= 0) {
                        // Rounding to 1 decimal place and also converting from pixel length to meters
                        std::string distStr = std::to_string(currentPath.distance * 1.396765498);
                        distStr = distStr.substr(0, distStr.find(".") + 2);
                        infoText.setString("Shortest Path Approximately: " + distStr + " meters");
                    }
                    else {
                        infoText.setString("Shortest Path Approximately: No route found");
                    }
                    // shows numerical value of the path's weights combined
                    infoText.setPosition({ 1700, 1300 });
                    window.draw(infoText);
                }
            }
        }
        // Show the new frame
        window.display();
    }
    return 0;
}