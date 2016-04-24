function [params] = nbayes_learn(traindata,trainlabels)
% function [params] = nbayeslearn(traindata,trainlabels)
%
%  function to learn from data the probabilities for a naive Bayes model 
%
%  Inputs
%     traindata: a n x d data matrix, d features/variables and n observations
%     trainlabels: a n x 1 vector of classlabels for traindata  
%     [Note to 177 students: you can if you wish assume that all variables
%      are binary, taking values = 1 or 2]
% 
%  Output
%     params: a stucture for the parameters for a Naive Bayes classifier
%     params(j).mprobs(k): probability that variable j takes value k
%     params(j).cprobs(k,i): conditional probability
%                          of prob(variable j = value k | class = value i) 
%     params(1).classprobs(i): probability that class variable = i 



% Feel free to write this code any way you like.  The outline below is
% just one example of how it can be done.

% ... define global parameters, etc............ 
nclasses = 2;
d = ....

% Estimate class probabilities and conditional probabilities  
for i=1:nclasses
    index = trainlabels==i; 
    params(1).classprobs(i) = ............;  % estimate the probability of class i 
    ......
    for j = 1:d 
        .......
        for k=1:2; 
            .......
            % estimate prob(variable j = k | class i)
            params(j).cprobs(k,i) = ..........
        end
    end
end
 
% Estimate the probabilities p(x_j = value_k)
for j = 1:d 
        ..............
        for k=1:2; 
            ......
            % estimate  prob(variable x_j = k)
            params(j).mprobs(k) = ........;
        end
  end
 

  
     
        
