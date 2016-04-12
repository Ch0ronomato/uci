
%
% This script gives some useful MATLAB commands for completing homework 1
%
% CS177, Spring 2009
%

% the parameter settings we would like to plot
n = 20;
p = 0.1;

% the range of values for which we want to plot the pmf
domain = 0:n

% now generate a vectory y where each value corresponds to 
% p(i) for i in 0..n
y = binomial_pmf(domain,n,p);


% plot the results using a bar graph with nice axis labels
% and a title at the top
bar(domain,y)
xlabel('i','FontSize',14);
ylabel('p(i)','FontSize',14);
title('Binomial Probability Mass Function: n = 20, p = 0.1','FontSize',14);


% plot the results using a line plot instead
%   this is useful for large values of n
plot(domain,y);
xlabel('i','FontSize',14);
ylabel('p(i)','FontSize',14);


