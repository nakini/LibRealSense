% Code for calculating error on Homogeneous tranformations.

% ==== Discourse on the scope and purpose of the following algorithm ==== %

% Calibration toolbox returns a transformation between each camera and the
% pattern (c[i]Hp[i]) and then the final tranformation between the 2 cameras in the
% stereo case.
% From this c[i]Hp[i] we can get each a tranformation between c[i]Hc[j] wrt
% pattern i,j,k, ..., etc.
% Then, we need to get the error between, c[i]Hc[j] p = 1,2,3, .. etc with
% respect to p=1 fixed. i.e error between the HTM between c1 and c2 wrt patern 1
% vs wrt pattern 2



% function call to DeltaRT, takes 2 structures with a 3x3 rotation matrix and
% a 3x1 translation matrix and calculates the error between them

%This is how to place the data in a struct, necessary for the DeltaRT
%function
clc;clear
path = '/home/cornea/goowfd/Documents/PhD/Computer_Vision/Data_Lab_calib_12_22_2020/115/';
load ([path 'Calib_Results_Up.mat']);
% Load the Calibration results and then store the necessary variables in a
% datastructure of choice.
RT_left(1).R = Rc_1;
RT_left(1).T = Tc_1;

RT_left(2).R = Rc_2;
RT_left(2).T = Tc_2;

RT_left(3).R = Rc_3;
RT_left(3).T = Tc_3;

RT_left(4).R = Rc_4;
RT_left(4).T = Tc_4;

RT_left(5).R = Rc_5;
RT_left(5).T = Tc_5;

RT_left(6).R = Rc_6;
RT_left(6).T = Tc_6;

RT_left(7).R = Rc_7;
RT_left(7).T = Tc_7;

RT_left(8).R = Rc_9;
RT_left(8).T = Tc_9;

% RT_left(9).R = Rc_9;
% RT_left(9).T = Tc_9;
% 
% RT_left(10).R = Rc_10;
% RT_left(10).T = Tc_10;
% 
% RT_left(11).R = Rc_11;
% RT_left(11).T = Tc_11;
% 
% RT_left(12).R = Rc_12;
% RT_left(12).T = Tc_12;
% 
% RT_left(13).R = Rc_13;
% RT_left(13).T = Tc_13;
% 
% RT_left(14).R = Rc_14;
% RT_left(14).T = Tc_14;

load ([path 'Calib_Results_Down.mat']);
% Load the Calibration results and then store the necessary variables in a
% datastructure of choice.

RT_right(1).R = Rc_1;
RT_right(1).T = Tc_1;

RT_right(2).R = Rc_2;
RT_right(2).T = Tc_2;

RT_right(3).R = Rc_3;
RT_right(3).T = Tc_3;

RT_right(4).R = Rc_4;
RT_right(4).T = Tc_4;

RT_right(5).R = Rc_5;
RT_right(5).T = Tc_5;

RT_right(6).R = Rc_6;
RT_right(6).T = Tc_6;

RT_right(7).R = Rc_7;
RT_right(7).T = Tc_7;

RT_right(8).R = Rc_9;
RT_right(8).T = Tc_9;

% RT_right(9).R = Rc_9;
% RT_right(9).T = Tc_9;
% 
% RT_right(10).R = Rc_10;
% RT_right(10).T = Tc_10;
% 
% RT_right(11).R = Rc_11;
% RT_right(11).T = Tc_11;
% 
% RT_right(12).R = Rc_12;
% RT_right(12).T = Tc_12;
% 
% RT_right(13).R = Rc_13;
% RT_right(13).T = Tc_13;
% 
% RT_right(14).R = Rc_14;
% RT_right(14).T = Tc_14;

num_patterns = length(RT_left);
num_cameras = 2; % Left and Right (Stereo Calib)
LeftHRight = {1,num_patterns};
deltaRPY = zeros(1,num_patterns);
deltaXYZ = zeros(1,num_patterns);
ref = 1;
%for cam_id = 1:num_cameras
    for pattern_id = 1:num_patterns
         fprintf("Pattern %d vs %d\nError Matrix:\n",ref,pattern_id);
         %Given a transformation from Left(up) camera to Pattern[pattern_id] and a transformation 
         %from Right (down) Camera to Pattern[pattern_id], we generate a transformation from Left(up) to
         % right(down) wrt Pattern[pattern_id] --> 4x4 HTM
         LeftHRight{pattern_id} = HTM_cam_LR_pattern(RT_left(pattern_id),RT_right(pattern_id));
         
         %Calculate the change in rotation and translation  between Left_H_Right_p1 and Left_H_Right_p[i]
         [dRPY,dXYZ] = DeltaRT_new(LeftHRight{ref},LeftHRight{pattern_id}); % Keep pattern 1 fixed!
         fprintf("Pattern %d vs %d | Error in Rotation(RPY):",ref,pattern_id);
         disp(dRPY);
         fprintf("Pattern %d vs %d | Error in Translation:",ref,pattern_id);
         disp(dXYZ);
         deltaRPY(pattern_id) = norm(dRPY); 
         deltaXYZ(pattern_id) = norm(dXYZ);
         %deltaRT(pattern_id).R = deltaRPY;
         %deltaRT(pattern_id).T = deltaXYZ;
    end
%end
   % This should give us the minimum 2-norm
   [minRPY,minRPY_index] = min(deltaRPY(2:end));
   [minXYZ,minXYZ_index] = min(deltaXYZ(2:end));
% Now that we have the HTM_error, then what??
fprintf("Pattern with lowest error with respect to pattern 1 in Rotation: %d\n",minRPY_index+1);
fprintf("Pattern with lowest error with respect to pattern 2 in Translation: %d\n",minXYZ_index+1);

% We want to average the Rotations and the Translations
% first, convert 3x3 rotations to 3x1 EULER, then 3x1 EULER to QUARTERNION
% Average the quartenion, if you do this, you lose orthogonality?!!
average_rot = [0 0 0 0];
average_tran = [0 0 0];

for iter = 1 : num_patterns
    HTM_RT = LeftHRight{iter};
    RPY = HTM_RT(1:3,1:3);
    Tr = HTM_RT(1:3,4)';
    euler_angles = rotm2eul(RPY); % This is in radians
    quat = eul2quat(euler_angles,'ZYX');
    average_tran = average_tran + Tr;
    average_rot = average_rot + quat;
end

average_tran = average_tran ./num_patterns;
average_rot = average_rot ./num_patterns;
