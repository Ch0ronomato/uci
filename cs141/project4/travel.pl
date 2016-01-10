nonStopTrain(sandiego,oceanside).
nonStopTrain(lasvegas,sandiego).
nonStopTrain(sanfrancisco,bakersfield).
nonStopTrain(bakersfield,sandiego).
nonStopTrain(oceanside,losangeles).
nonStopTrain(portland,sanfrancisco).
nonStopTrain(seattle,portland).

routeTrip(X,Y,[X|Trip]) :-
	nonStopTrain(X, Z),
	routeTrip(Z, Y, Trip).
routeTrip(X,Y,[X,Y]) :- nonStopTrain(X, Y).
