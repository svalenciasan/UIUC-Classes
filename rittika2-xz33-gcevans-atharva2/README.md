# CS 225 Staff Final Project
## Team Members: Rittika Adhikari, Eunice Zhou, Graham Carl Evans, Brad Solomon

### Github Workflow Cheatsheet
*For a more advanced Github Workflow option, check out https://jameschambers.co.uk/git-team-workflow-cheatsheet*
1. Whenever you sit down to work, `git pull origin master`. 
2. Edit/Create your files. When you're finished, make sure to `git add <file name>`. 
3. Ensure that all files that you want to be tracked have been added using `git status`. 
    1. Tracked files are in ![#c5f015](https://via.placeholder.com/15/c5f015/000000?text=+) `green`. 
    2. Untracked files are in ![#f03c15](https://via.placeholder.com/15/f03c15/000000?text=+) `red`.
4. To commit your changes, `git commit -m "<your descriptive commit message>"`. 
5. Push your changes to the master branch using `git push origin master`. 
6. If you forget to pull, `git stash` your changes, then `git stash apply`. If there is a merge conflict manually resolve it, and `git add <conflict file>`.
7. If you run into a merge conflict, `git pull origin master` and manually resolve it.

### Guide to Catch Testing
*For an example of Catch testing, look at `tests/test.cpp`.*
* Incremental Testing: test each function you write before using them 
* Test early and test often
* Test with smaller samples of data first to make sure the algorithm works as expected
* Work out the expected output and use assertions to compare the result (for simple cases)
* Some examples/ideas:
    * Check that every data point is covered in the traversal
    * Check for ordering in small samples (i.e. think about testing in MP Traversals)

## How to Read a File?
Check out `readFromFile.hpp` and `readFromFile.cpp`! For an example of the functions being used, take a look at `main.cpp`.

## How do I _Compile_ My Code? [Help, I Forgot MP Intro!]
Check out the sample `Makefile` provided, and feel free to use it as a template :) 

## Tips and Tricks
* Don't commit all of your data (especially if your dataset is big). Instead, commit a _small sample_ and add a link to where you found your dataset in your README.md.
* Don't forget to consider your data file format. Include what formats you expect in your function docstrings.
* Test early!!! I cannot stress this enough.
* Designate responsibility & hard deadlines to each team member. Parallel development sessions are highly encouraged.
