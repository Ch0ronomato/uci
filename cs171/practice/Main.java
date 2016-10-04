package practice;
import java.io.*;
import java.util.ArrayList;
import practice.Tree;
public class Main {
    public static void main(String[] args) {
        int[] data = new int[args.length];
        for (int i = 0; i < args.length; i++)
            data[i] = (int)Double.parseDouble(args[i]);
        new Main().start(data);
    }
    private Tree tree;
    public void start(int[] args) {
        this.tree = new Tree(args, 5);
        this.iterativeDeepeningSearch(args[903]);
    }

    private Tree.Node iterativeDeepeningSearch(int value) {
        Tree.Node found = null;
        for (int i = 0; i < 10; i++) {
            System.out.println("Depth of " + i);
            found = iterativeDeepeningSearch(this.tree.getRoot(), value, i);
            if (found != null)
                break;
            System.out.println("-------------------------");
        }
        System.out.print("found ");
        if (found == null)
            System.out.println("nothing");
        else
            System.out.println(found.data);
        return found;
    }

    private Tree.Node iterativeDeepeningSearch(Tree.Node node, int value, int depth) {
        if (depth <= 0) return null;
        else if (node.data == value)
            return node;
        else {
            for (Tree.Node child : node.children) {
               Tree.Node found = iterativeDeepeningSearch(child, value, depth - 1); 
                if (found != null) return found;
            }
            return null;
        }    
    }
}
