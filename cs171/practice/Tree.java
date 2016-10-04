package practice;
import java.io.*;
import java.util.ArrayList;
import java.util.LinkedList;
class Tree {
    public class Node {
        public int data;
        public Node parent;
        public ArrayList<Node> children;
        public Node(int data, Node parent, ArrayList<Node> children) {
            this.data = data;
            this.parent = parent;
            this.children = children;
        }
        public Node(int data, Node parent) {
            this.data = data;
            this.parent = parent;
            this.children = new ArrayList<>(); 
        }
    }
    private Node root;
    int N;
    public Tree(int data[], int N) {
        // creates a tree of random ordering
        this.root = new Node(data[0], null);
        this.N = N; 
        LinkedList<Node> addQueue = new LinkedList<>();
        addQueue.add(this.root);
        int i = 1;
        while (addQueue.size() > 0 && i < data.length) {
            Node parent = addQueue.poll();
            for (int childi = 0; i < data.length && childi < N; childi++) {
                Node n = new Node(data[i++], parent);
                parent.children.add(n);
                addQueue.add(n);
            }
        }
    }

    public Node getRoot() {
        return this.root;
    }

    public void print() {
        print(this.root, ""); 
    }

    private void print(Node toprint, String sep) {
        int left = this.N / 2;
        for (int i = 0; i < toprint.children.size() && i < left; i++) 
            print(toprint.children.get(i), sep + "\t");
        System.out.println(sep + toprint.data);
        for (int i = left; i < toprint.children.size()&& i < toprint.children.size(); i++) {
            print(toprint.children.get(i), sep + "\t");
        }
        if (toprint.children.size() > 0) System.out.println();
    }
}
