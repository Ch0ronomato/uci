%
% A MATLAB script to visualize different probability
% models for the website session length data
%
% <yournamehere>   CS177, Spring 2009

load sessionlengths.txt;

% first estimate the parameter p for the geometric distribution

... your code goes here ...

% define the values for gamma and the normalization constant
% for the power-law distribution

gamma = 2;
Z = pi.^2 / 6;




% now compute the pmf for the three different distributions

% values to compute the pmf at
nvalues = 1:50

for i = 1:length(nvalues)
  
  n = nvalues(i);

  pmf_empirical(i) = ...
  pmf_geometric(i) = ...
  pmf_power_law(i) = ...

end


% now plot the 3 different graphs


% first graph is linear axes.  plot the three distributions
% with different symbols for each line

figure(1);
plot(nvalues,pmf_empirical,'bo');
hold on; %stops matlab from erasing the previous plot when drawing a new one
plot(nvalues,pmf_geometric,'r:');
plot(nvalues,pmf_power_law,'g-');

legend('empirical','geometric','power-law');
xlabel('n');
ylabel('P(n)');
title('pmf for internet session length data');



figure(2);

...


figure(3);


...


