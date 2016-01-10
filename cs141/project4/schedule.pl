team(connecticut, 22, 2, east).
team(duke, 24, 1, east).
team(memphis, 23, 2, south).
team(villanova, 20, 2, east).
team(gonzaga, 21, 3, west).
team(texas, 22, 3, south).
team(georgeWashington, 20, 1, east).
team(tennessee, 18, 3, south).
team(pittsburgh, 19, 3, midwest).
team(florida, 21, 3, east).
team(westVirginia, 18, 6, east).
team(ohioState, 18, 3, midwest).
team(bostoncollege, 20, 5, east).
team(illinois, 20, 4, midwest).
team(uCLA, 20, 5, west).
team(michiganState, 18, 7, midwest).
team(georgetown, 17, 5, east).
team(iowa, 20, 6, midwest).
team(oklahoma, 16, 5, midwest).
team(washington, 18, 5, west).
team(northCarolinaState, 19, 5, east).
team(kansas, 18, 6, midwest).
team(northCarolina, 15, 6, east).
team(bucknell, 20, 3, east).
closeto(west, midwest).
closeto(east, midwest).
closeto(midwest, south).
closeto(east, south).



zone(X, Y) :-
	team(X, _, _, Z),
	team(Y, _, _, Z).
zone(X, Y) :-
	team(X, _, _, Z1),
	team(Y, _, _, Z2),
	closeto(Z1, Z2).
pairings([], [], L, []).
pairings([A], [A], L, [A]).
pairings([First|Teams], [Last|RevTeams], Pairs, Leftovers) :-
	last(First, TeamX),
	last(Last, TeamY),
	append([[TeamY, TeamX]], Pairs, NewPairs),
	pairings(Teams, RevTeams, NewPairs, Leftovers).
doPairings([X|_]) :-
	reverse(X, Y),
	pairings(X, Y, NewPairs, Leftovers).
group() :-
	findall(Q, setof([W, N], N^L^team(N, W, L, west), Q), X),
	doPairings(X).

