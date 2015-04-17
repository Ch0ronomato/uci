<html><head>
  <title>Program 2</title>
</head>

<body bgcolor="white">

<center>
<h1>Program 2</h1>
<h1>Implementing Queue/Priority Queue/Set<br>
    (and their iterators) with Linked Lists 
</h1>
<p>
</p><h2>ICS-46: Data Strcuture Implementation and Analysis
</h2>
<p>
</p></center>


<!-- Introduction -->

<a name="Introduction">
<hr align="left" width="33%">
<table cellspacing="0" cellpadding="5" border="0" rules="none" width="100%">
<tbody>
<tr valign="top">
<td width="20%"><b>Introduction</b></td>
<td width="80%">
This programming assignment is designed to ensure that you know how to
  implement three templated classes (<b>Queue</b>, <b>Priority Queue</b>,
  and <b>Set</b>) with linked lists.
Your implementations will also include fully-functional iterators for
  these classes.
You will be writing lots of small code fragments that traverse and mutate
  linked lists.
<p>
You must write all your implementations using linked lists: a linear-linked
  list for <b>LinkedQueue</b>, a header linked list for
  <b>LinkedPriorityQueue</b>, and a trailer linked list for <b>LinkedSet</b>.
You can test these implementations by using the standard drivers and
  GoogleTests (provided with the download) that we will use when grading your
  code for correctness; recall that you can augment the GoogleTest with whatever
  code you want, to aid your debugging: a GoogleTest is just a C++ program.
You can also test the code you wrote for Programming Assignment #1 (using
  array implementations of these classes) by substituting these linked list
  implementations -typically by changing a few <b>#include</b> and
  <b>typedef</b> statements.
</p><p>
Write and use the <b>insertion</b> (<b>&lt;&lt;</b>) operator and <b>str()</b>
  method in each class for debugging.
In a header list, we skip showing the value in the front/header node, as that
  node is not really <b>in</b> the collection represented by the list;
  likewise, in a trailer list, we skip showing the value in the rear/trailer
  node, for the same reason.
Note that there is no tested requirement for what these methods return, but
  the versions above will make debugging easier.
</p><p>
You should download the 
  <a href="program2.zip">program2</a> project folder and use it to create an
  Eclipse project (ultimately needing to connect it to both the
  <b>courselib</b> and <b>googletest</b> libraries).
  You will write the required methods in the <b>linked_queue.hpp</b>, 
  <b>linked_priority_queue.hpp</b>, and <b>linked_set.hpp</b>, files in this
  project, and submit each separately in Checkmate.
The project folder also contains three pairs of <b>.hpp</b> and <b>.cpp</b>
  files: a driver/GoogleTest pair for each class that you will write, and the
  <b>driver.cpp</b> file which has a <b>main</b> function that can be made to
  run any of the three drivers.
</p><p>
Instead, you can also use a existing/working project folder that already is
 connected to both the <b>courselib</b> and <b>googletest</b> libraries: remove
  (but save) all the files in its <b>src</b> folder and then put all the
  <b>.hpp</b> and <b>.cpp</b> files from the downloaded project's <b>src</b>
  folder into the existing/working project's <b>src</b> folder; finally,
  right-click the project and select <b>Refresh</b> (F5).
</p><p>
<b>Important: Only one of the <b>.cpp</b> files with a <b>main</b> method can
  be active/compiling at any time.</b>
In the download, only the <b>driver.cpp</b> file is active; the GoogleTests are
  inactive.
To make a progam inactive, select it (in the editor tab), use the <b>Ctrl/a</b>
  command to select all its lines, and then click <b>Source</b> at the top
  left of the menu and choose <b>Toggle Comment</b>: ever line will now appear
  in a comment (so the <b>main</b> function is commented-out); by using these
  same instructions, you can toggle back those lines to not have comments.
</p><p>
I recommend that you work on this assignment in pairs.
Try to find someone who lives near you, with similar programming skills,
  and work habits/schedule: e.g., talk about whether you prefer to work
  mornings, nights, or weekends; what kind of commitment you will make to submit
  program early.
</p><p>
<b>Only one student should submit all parts of the the assignment</b>, but both
  student's names (along with their UniqueID) should appear in the comments at
  the top of <b>each submitted .cpp</b> file.
It should look something like
</p><pre><b>
//Romeo Montague(UniqueID from grades spreadsheet)
//Juliet Capulet(UniqueID from grades spreadsheet)
//We certify that we worked cooperatively on this programming
//  assignment, according to the rules for pair programming</b></pre>
If you do not know what the terms <b>cooperatively</b> and/or
  <b>rules for pair programming</b> mean, please read about
  <a href="../../../common/handouts/pairprogramming.html">Pair Programming</a> 
  before starting this assignment.
Please turn in each program <b>as you finish it</b>, so that I can accurately
  assess the progress of the class as a whole during this assignment.
<p>
Print this document and carefully read it, marking any parts that contain
  important detailed information that you find (for review before you turn in
  the files).
</p><p>
</p><p>
This assignment has 3 parts: pairs should work on each part together, not split
  them up and do them separately.
Part 1 is worth 42 points; part 2 is worth 9 points; part 3 is worth 9 pts.

This skewing of points towards the simpler parts means students finishing the
  first part correctly will have a 70% average; those finishing the first 2 
  parts correctly will have an 85% average; but to get an A on this 
  assignment requires solving all parts correctly.
Remember I'm going to be running MOSS on the parts of this assignment,
  to check for program similarity.
</p><p>
IMPORTANT: The courselib contains array implementations for all these data
  types; although this assignment requires you to use linked lists, there are
  still <b>many strong similarities</b> at a high level in all these
  implementations.
So, I encourage you to examine these implementations closely, and understand
  them; possibly, experiment with them (using their drivers or GoogleTests),
  while you are writing your linked list implementations: this advice is
  especially true as you begin to study, understand, and implement iterators.
Please feel free about asking questions about these methods -both their syntax
  and semantics- on the appropriate Messageboard.
</p></td>
</tr></tbody>
</table>


<!-- queue -->

</a><a name="queue">
<hr align="left" width="33%">
<table cellspacing="0" cellpadding="5" border="0" rules="none" width="100%">
<tbody>
<tr valign="top">
<td width="20%"><b>Queues</b></td>
<td width="80%">
Queues are implemented by simple FIFO data structures (adhering to the
  Fast-In/First-Out order property).
We can implement queues efficiently by using two instance variables, which
  refer to a linked list (whose first value is the node at the <b>front</b>
  of the queue and whose last value is the node at the <b>rear</b> of the
  queue).
Nodes are removed from the front and added to the rear, so these are the
  two "hot spots" for a queue.
The <b>enqueue</b> and <b>dequeue</b> operations should each be O(1).
<p>
Although we can easily compute the number of values in linked list by
  traversing it, instead we will declare and update an extra instance variable 
  named <b>used</b> to cache the size (incrementing and decrementing it, as
  values are successfully added/removed from the queue), so we will not have
  to traverse the list to compute its size.
</p><p>
The file <b>linked_queue.hpp</b> declares the appropriate constructors, methods,
  and instance variables.
Notice how the <b>LN</b> class is first declared <b>private</b> (before
  <b>Iterator</b>) and then defined <b>private</b> inside <b>LinkedQueue</b>
  using the parameter <b>&lt;T&gt;</b> of that class.
I suggest copying/pasting the methods from the <b>array_queue.hpp</b>
  file, and then translating these methods from using arrays to using linked
  lists.
Pay close attention to ensure <b>all instance variables</b> receive values in
  the constructors and are used/set correctly in queries and commands.
</p><p>
Use delete to recycle <b>LN</b> nodes.
To simplify the operator=, you can <b>clear</b> the queue and then
  <b>enqueue</b> the required values: a faster way would be to clear the queue
  and copy the linked list; an even faster way would reuse whate <b>LN</b>
  nodes are already there, removing unneeded one or supplementing with more
  nodes.
</p><p>
Read the material in the <b>Iterators</b> section of this assignment, which
  discusses the iterators needed for all the classes that you will write.
These iterators perform the the same operations in every class, but they are
  implemented differently based on the kind of linked list implementation.
Finally, read the <b>testing</b> section below as well.
</p></td>
</tr></tbody>
</table>



<!-- priorityqueue -->

</a><a name="priorityqueue">
<hr align="left" width="33%">
<table cellspacing="0" cellpadding="5" border="0" rules="none" width="100%">
<tbody>
<tr valign="top">
<td width="20%"><b>Priority Queues</b></td>
<td width="80%">
Priority Queues can be implemented by a variety of data structures (where the
  highest priority value is always removed first).
How does a specific priority queue determine which value has the highest
  priority?
When constructed, we supply the priority queue with a <b>gt</b> (greater-than)
  function that computes whether its first argument has a greater priority
  than its second argument.
So, we cannot ask, "What is the priority of a value." But, we can ask "Does the
  priority of a first value have a higher priority than a second value", by
  calling the <b>gt</b> function.
For example, we cannot ask for the priority of a <b>std::string</b> value, but
  we can determine whether one <b>std::string</b> value has a higher
  priority than another <b>std::string</b> value.
<p>
We can implement priority queues simple (although not very efficiently) with
  one instance variable, which refers to a linked list whose first value is the
  one with the highest priority value, and whose remaining values occur in
  order of decreasing priority; when adding a value to a priority queue, we
  insert it at the correct spot in the list, keeping the list ordered from
  highest to lowest priority; when removing the highest priority value from a
  priority queue, we always remove it from the front.
</p><p>
Instead of a standard linked list, you must <b>implement the priority queue
  using a "Header node" at the front of the linked list.</b>
Doing so should simplify writing the most complicated method: enqueuing an
  element onto the priority queue: try to write this method very simply.
Hint: my <b>enqueue</b> method used four lines to put the <b>element</b> into
  an <b>LN</b> and insert it into the correct position in the header linked
  list (followed by 3 more lines of booking code).
</p><p>
Although we can easily compute the number of values in linked list by
  traversing it, instead we will declare and update an extra instance variable 
  named <b>used</b> to cache the size (incrementing and decrementing it, as
  values are successfully added/removed from the queue), so we will not have
  to traverse the list to compute its size.
</p><p>
The <b>enqueue</b> operation is O(N), so enqueuing N values onto a priority
  queue is O(N^2).
When writing the copy constructor and operator=, use the fact that "the linked
  lists being copied are already in order" to make these operations O(N).
To simplify operator=, you can <b>clear</b> the queue and then carefully add
  the required values (in linear time).
</p><p>
The file <b>linked_priority_queue.hpp</b> declares the appropriate constructors,
  methods, and instance variables.
Notice how the <b>LN</b> class is first declared <b>private</b> (before
  <b>Iterator</b>) and then defined <b>private</b> inside
  <b>LinkedPriorityQueue</b> using the parameter <b>&lt;T&gt;</b> of that class.
I suggest copying/pasting the methods from the <b>array_priority_queue.hpp</b>
  file, and then translating these methods from using arrays to using linked
  lists; in fact, you might want to copy the <b>linked_queue.hpp</b> file you
  wrote in part 1: it has the same methods and it already does some linked list
  processing: but doesn't use a header linked list.
Pay close attention to ensure <b>all instance variables</b> receive values in
  the constructors are are used/set correctly in queries and commands.
Use delete to recycle <b>LN</b> nodes.
</p><p>
Read the material in the <b>Iterators</b> section of this assignment, which
  discusses the iterators needed for all the classes that you will write.
These iterators perform the the same operations in every class, but they are
  implemented differently based on the kind of linked list implementation.
Finally, read the <b>testing</b> section below as well.
</p></td>
</tr></tbody>
</table>


<!-- set -->

</a><a name="set">
<hr align="left" width="33%">
<table cellspacing="0" cellpadding="5" border="0" rules="none" width="100%">
<tbody>
<tr valign="top">
<td width="20%"><b>Sets</b></td>
<td width="80%">
Sets can be implemented by a variety of data structures.
We can implement sets simply (although not very efficiently) with one instance
  variable, which refers to a linked list of values in the set.
Remember that a set's order is not important: e.g., when we <b>insert</b> an
  <b>element</b> into a <b>set</b>, we are free to put it anywhere in the
  linked list; put it somewhere easy by using short/efficient code.
<p>
Instead of a standard linked-list, you must <b>implement the set using a
  "Trailer node" last in the linked list.</b>
In fact, it is useful to declare a <b>trailer</b> instance variable that always
  refers to this node (be careful: erasing the value in the node before the
  trailer node requires changing the trailer node; also, every linked list has
  its own/different trailer node).
Having a trailer node should simplify erasing a value from the set (in the
  class method, but even more so in the iterator methods, using the standard
  code/trick covered in the discussion of trailer lists.
</p><p>
Although we can easily compute the number of values in linked list by
  traversing it, instead we will declare and update an extra instance variable 
  named <b>used</b> to cache the size (incrementing and decrementing it, as
  values are successfully added/removed from the set), so we will not have
  to traverse the list to compute this value.
</p><p>
The <b>insert</b> operation is O(N), because it must check if the element is
  already in the set, so inserting N values into a set is O(N^2).
When writing the copy constructor and operator=, use the fact that "the linked
  lists being copied are already sets with no duplicates" to make these 
 operations O(N).
To simplify operator=, you can <b>clear</b> the set and then carefully add
  the required values (in linear time).
</p><p>
The file <b>linked_set.hpp</b> declares the appropriate constructors,
  methods, and instance variables.
Notice how the <b>LN</b> class is first declared <b>private</b> (before
  <b>Iterator</b>) and then defined <b>private</b> inside <b>LinkedSet</b>
  using the parameter <b>&lt;T&gt;</b> of that class.
I suggest copying/pasting the methods from the <b>array_set.hpp</b> file,
  and then translating these methods from using arrays to using linked lists.
Pay close attention to ensure <b>all instance variables</b> receive values in
  the constructors are are used/set correctly in queries and commands.
Use delete to recycle <b>LN</b> nodes.
</p><p>
Read the material in the <b>Iterators</b> section of this assignment, which
  discusses the iterators needed for all the classes that you will write.
These iterators perform the the same operations in every class, but they are
  implemented differently based on the kind of linked list implementation.
Finally, read the <b>testing</b> section below as well.
</p></td>
</tr></tbody>
</table>



<!-- iterators -->

</a><a name="iterators">
<hr align="left" width="33%">
<table cellspacing="0" cellpadding="5" border="0" rules="none" width="100%">
<tbody>
<tr valign="top">
<td width="20%"><b>Iterators</b></td>
<td width="80%">
Fundamentally, iterators operate similarly for each data type: they allow
  programmers to traverse the data type, examining (and even erasing) the
  values inside the data type, one after the other.
Each uses a cursor to remember its place inside the data type, as it traverses
  it: the array implementations used <b>int</b>s for cursors; the linked-list
  implementations use pointers for cursors.
<p>
Once iterators are created (indexing the first value, or if there is none,
  indexing "one beyond the last value") we can use them to examine/erase the
  current value, increment them (to access the next value),  and check whether
  a cursor has reached "one beyond the last value".
We often do this with either explict <b>for</b> loops or with implicit
  <b>for-each</b> loops (we do the latter more frequently).
</p><p>
Note that if we <b>erase</b> a value, the cursor will temporarily refer to
  its next value: we <b>must call one form of the ++ operator to increment the
  cursor</b> before we can examine/erase another value, but doing so does right
  after calling <b>erase</b> will not change the cursor, because <b>erase</b>
  has already made the cursor refer to its next value.
</p><p>
In the <b>linked_queue.hpp</b> and <b>linked_priority_queue.hpp</b> files, I
  have implemented all <b>Iterator</b> methods fully except for the two forms
  of the <b>++</b> operator and the <b>erase</b> method, and for these I have
  even written some of the boiler-plate code.
</p><ol>
<li>Each defines a <b>prev</b> and <b>current</b> instance variable of type
  <b>LN*</b> (to traverse their linked list), along with a <b>ref_queue</b>
  (in the queue) or a <b>ref_pq</b> (in the priority queue) pointer referring
  to the object they are iterating over (to access its current <b>mod_count</b>
  and other instance variables as needed).
<p>
</p></li><li>Each defines an <b>expected_mod_count</b> and <b>can_erase</b> instance
  variable. Note that <b>can_erase</b> determines whether <b>erase</b> can
  function correctly, and helps control the updating of <b>prev</b> and
  <b>current</b> after calling <b>++</b> on an iterator.
  Note that <b>prev</b> is useful when the <b>current</b> value must be erased.
<p>
</p></li><li>All methods start with similar tests that determine whether to throw an
      exception. 
    Most compare <b>mod_count</b> and <b>expected_mod_count</b>.
    The <b>==</b> and <b>!=</b> relational operators also ensure what is being
      compared are iterators from the same object.
    The <b>*</b> and <b>-&gt;</b> operators ensure that <b>current</b> is
      legal to examine: is not one beyond the last value.
</li></ol>  
Observe their similarity in all implementations among all the <b>Iterator</b>
  classes and their methods.
I recommend writing and testing the code for both <b>++</b> operators before
  writing code for an iterator's <b>erase</b> method.
This will allow you to test loops using iterators, so long as the body of the
  loop does not call <b>erase</b> on the iterator.
After your code for these operators is working correctly, write code for
  calling <b>erase</b> on an iterator, and update the code in the <b>++</b>
  operators where necessary, to work correctly with <b>erase</b>.
<p>
Note that iterators for these two classes produce values in the order that they
  would be dequeued: FIFO for a queue and priority ordering for a priority
  queue.
Given how these linked lists represent these classes (queue: front to rear;
  priority queue: highest to lowest priority), the order of iterating through
  these classses is the same as the order of traversing their linked list
  implementation from front to rear.
</p><p>
There are four GoogleTest functions that focus on iterators:
<b>iterator_plusplus</b> focuses on the two forms of <b>++</b> operators
  and does <b>not</b> call the iterator's <b>erase</b>;
<b>iterator_simple</b> does <b>not</b> call the iterator's <b>erase</b>;
<b>iterator_erase</b> does call the iterator's <b>erase</b>; and
<b>iterator__exception_concurrent_modification_error</b> ensures that mutating
  the data structure forces any active iterator to stop working (unless the
  mutation was done by that iterator's <b>erase</b>).
</p><p>
For the <b>linked_set</b> class you must write all the code for the iterator.
The use of a trailer list will often make such code easier to write: it
  requires only a <b>current</b> instance variable, not one for <b>prev</b>.
In general, you should hand-simulate/debug your iterator code for the following
  cases:
</p><ol>
  <li> erasing the first value (maybe several in a row at the front)
  </li><li> erasing non-consecutive values inside (with multiple <b>++</b>
         operators between calls)
  </li><li> erasing consecutive values  (with single <b>++</b>
         operators between calls)
  </li><li> erasing the last value (maybe several in a row at the end)
</li></ol>
<p>
You can study how these semantics are coded in the array implementations of
  these data types, which are similiar but simpler than how they are coded with
  linked lists (because we can more easily manipulate <b>int</b> array cursors:
   <b>i-1</b> and <b>i+1</b>).
For linked list implementations, implementing <b>erase</b> on iterators is
  typically more complicated, but more EFFICIENT: values removed in the middle
  of arrays require shifting, causing the complexity class of <b>erase</b>
  to be O(N) in arrays while being only O(1) in linked lists.
</p></td>
</tr></tbody>
</table>


<!-- testing -->

</a><a name="testing">
<hr align="left" width="33%">
<table cellspacing="0" cellpadding="5" border="0" rules="none" width="100%">
<tbody>
<tr valign="top">
<td width="20%"><b>Testing</b></td>
<td width="80%">
There are various ways to test each of the classes you are writing in this
  programming assignment.
First, though, you should write all the methods, paying careful attention to
  the array implementations and previously written linked list implementations.
For some, you might just boiler-plate simple code that is not correct, but
  allows the methods to compile, allowing other methods in the classes to be
  tested.
<p>
The easiest way to start testing//debugging is by using the driver program.
It allows you to perform any method call supported by the templated classes,
  and see the state of the class (or view the debugger).
Of course, 
<b>you must get the <b>insertion</b> (&lt;&lt;) operator and str() method to
  work before using it to debug the other methods.</b>
</p><p>
After you test and debug your code with the driver, try running the appropriate
  GoogleTest code.
Again, this form of testing is useful only as you approach a finished solution.
We will use the GoogleTest, and visual inspection, to grade this assignment.
<b>Important Note</b>: You can put <b>std::cout</b> statements in the GoogleTest
  code (but don't accidentally remove any of the assertions, otherwise
  you won't be fully checking your code the way we will) while you are
  debugging your classes.
All debugging <b>std::cout</b> should end in <b>std::endl</b> to flush the
  output stream: taht ensures the output it displayed before executing the next
  statement (which may throw an exception).
</p><p>
When you run the GoogleTest, initially choose small values for the first and
  third prompts (just press return to the second prompt) or replace these
  prompts with small values in the code (so the test will run fully
  automatically).
Besides an indication of which tests pass and fail, the console window
  will show a speed for the speed test (which will vary depending on how
  fast a machine you run your code on): don't worry about it.
When your code is passing all the tests, put in values like <b>10,000</b> for
  these prompts.
</p><p>

</p></td>
</tr></tbody>
</table>





</a></body></html>
