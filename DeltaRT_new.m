function [deltaRPY, deltaXYZ] = DeltaRT_new(oldA, newA)
% Given two transformation matrices, this function is going to find out the
% difference between them in terms of Roll-Pitch-Yaw for rotation and
% translation.
%
% INPUT(s)
% ========
% 1. oldA, newA: 4x4 Matrix -- It holds rotation and translation as 
% [R T]
% [0 1]
% OUTPUT(s)
% =========
% 1. deltaRPY: 1x3 vector -- It will hold the difference between two rotation
% matrices in the form of Raw, Pitch and Yaw
%
% 2. deltaXYZ: 1x3 vector -- It contains the difference in translation.
%
% Example(s)
% ==========
%
% Find the difference between the two transformation matrices.
deltaA = oldA \ newA;           %Transform from A2 to A1
disp(deltaA);
% Get the RPY and XYZ from the homogeneous matrix.
deltaXYZ = deltaA(1:3, 4)';
deltaRPY = rotm2eul(deltaA(1:3, 1:3)); % This is in radians
end