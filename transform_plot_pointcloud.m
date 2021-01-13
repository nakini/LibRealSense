
 clc;clear;close all
 pc1 = pcread('cam1_pc_cleaned.ply');
 pc2 = pcread('cam2_pc_cleaned.ply');


%pc1 = pcread('cam1_pc.ply');
%pc2 = pcread('cam2_pc.ply');

load 'Calib_Results_150_cam1.mat'
% Load the Calibration results and then store the necessary variables in a
% datastructure of choice.
RT_cam1(1).R = Rc_1;
RT_cam1(1).T = Tc_1;

RT_cam1(2).R = Rc_2;
RT_cam1(2).T = Tc_2;

RT_cam1(3).R = Rc_3;
RT_cam1(3).T = Tc_3;

RT_cam1(4).R = Rc_4;
RT_cam1(4).T = Tc_4;

RT_cam1(5).R = Rc_5;
RT_cam1(5).T = Tc_5;

RT_cam1(6).R = Rc_6;
RT_cam1(6).T = Tc_6;

RT_cam1(7).R = Rc_7;
RT_cam1(7).T = Tc_7;

RT_cam1(8).R = Rc_8;
RT_cam1(8).T = Tc_8;

RT_cam1(9).R = Rc_9;
RT_cam1(9).T = Tc_9;

load 'Calib_Results_150_cam2.mat'
% Load the Calibration results and then store the necessary variables in a
% datastructure of choice.

RT_cam2(1).R = Rc_1;
RT_cam2(1).T = Tc_1;

RT_cam2(2).R = Rc_2;
RT_cam2(2).T = Tc_2;

RT_cam2(3).R = Rc_3;
RT_cam2(3).T = Tc_3;

RT_cam2(4).R = Rc_4;
RT_cam2(4).T = Tc_4;

RT_cam2(5).R = Rc_5;
RT_cam2(5).T = Tc_5;

RT_cam2(6).R = Rc_6;
RT_cam2(6).T = Tc_6;

RT_cam2(7).R = Rc_7;
RT_cam2(7).T = Tc_7;

RT_cam2(8).R = Rc_8;
RT_cam2(8).T = Tc_8;

RT_cam2(9).R = Rc_9;
RT_cam2(9).T = Tc_9;



for i=6
%     htm1 = eye(4);
%     htm2 = eye(4);
%     htm1(1:3,1:3) = RT_left(i).R;
%     htm1(1:3,4) = RT_left(i).T;
%     
%     htm2(1:3,1:3) = RT_right(i).R;
%     htm2(1:3,4) = RT_right(i).T;
%     
%     htm1_inv = inv(htm1);
%     htm2_inv = inv(htm2);
%    
%     tform1 = rigid3d(htm2_inv(1:3,1:3), htm2_inv(1:3    ,4)');
%     tform2 = rigid3d(htm1_inv(1:3,1:3), htm1_inv(1:3,4)');
    c2Hc1 = HTM_cam_LR_pattern(RT_cam1(i),RT_cam2(i));
    pc1_to_cam2 = transform_pc(pc1.Location,c2Hc1);
    %pc2_out = transform_pc(pc2.Location,RT_right(i));
    pc1_new = transform_pc(pc1_to_cam2,RT_cam2(i)); % transform to pattern position i
    pc2_new = transform_pc(pc2.Location,RT_cam2(i));% transform to pattern position i
    pc1_new = pointCloud(pc1_new);
    pc2_new = pointCloud(pc2_new);
    figure,
    subplot(311),pcshow(pc1_new),title('Pointcloud 1'),
    subplot(312),pcshow(pc2_new),title('Pointcloud 2'),
    subplot(313),pcshowpair(pc2_new,pc1_new),title('Pc2 to Pc1');
end
%figure,pcshow(pc2_to_c1);
%figure,subplot(121),pcshow(pc1),title('PC 1'), subplot(122),pcshow(pc1_out),title('PC 1 Transformed'),xlabel('X'),ylabel('Y'),zlabel('Z');
%figure,subplot(121),pcshow(pc2),title('PC 2'), subplot(122),pcshow(pc2_out),title('PC 2 Transformed'),xlabel('X'),ylabel('Y'),zlabel('Z');
