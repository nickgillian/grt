##GRT Wiki Setup
The GRT wiki lives in its own repo: https://github.com/nickgillian/grt.wiki.git

It has been cloned as a submodule in this directory.  If you want to edit the wiki so you can contribute to the GRT, you can clone it as follows:

    #Clone the GRT
    git clone https://github.com/nickgillian/grt.git

    #Clone the wiki via git submodules
    cd grt/wiki
    git submodule init
    git submodule update
