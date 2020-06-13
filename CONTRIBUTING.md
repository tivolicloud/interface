# Contributing

1. Fork the repository in our own gitlab.
2. Clone your fork of the repository locally

  ```
  git clone https://git.tivolicloud.com/USERNAME/interface.git
  ```
3. Create a new branch
  
  ```
  git checkout -b new_branch_name 
  ```
4. Code
  * Follow the [coding standard](CODING_STANDARD.md)
5. Commit
  * Use [well formed commit messages](http://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html)
6. Update your branch
  
  ```
  git remote add upstream https://git.tivolicloud.com/tivolicloud/interface.git
  git pull upstream master
  ```
  
  Resolve any conflicts that arise with this step.
7. Push to your fork
  
  ```
  git push origin master
  ```
8. Submit a merge request

  *You can follow [Introduction to GitLab Flow](https://docs.gitlab.com/ee/topics/gitlab_flow.html) to find out how to create a merge request.*
  
## Reporting Bugs

1. Always update to the latest code on master. It is possible the bug has already been fixed!
2. Search in [issues](https://git.tivolicloud.com/tivolicloud/issues/-/issues) and [roadmap](https://roadmap.tivolicloud.com/) to make sure that somebody has not already reported the same bug.
3. Add a [issue](https://git.tivolicloud.com/tivolicloud/issues/-/issues/new?issue%5Bassignee_id%5D=&issue%5Bmilestone_id%5D=) including information about your system and how to reproduce the bug. Screenshots are always appreciated.

## Requesting a feature

1. Search the [roadmap](https://roadmap.tivolicloud.com/) to make sure that somebody has not already requested the same feature. If you find a matching request, feel free upvote it ansd to add any additional comments.
2. Add a [feature](https://roadmap.tivolicloud.com/) and includes a detailed description of your request. We will look at it shortly and categorise it.
