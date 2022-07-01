/**
 * Utilities for Using IHMC Messages
 * Emily Sheetz, NSTGRO VTE 2021
 **/

#include <ihmc_utils/ihmc_msg_utilities.h>

namespace IHMCMsgUtils {

    void testFunction() {
        std::cout << "IHMCMsgUtilities HELLO WORLD!" << std::endl;
        return;
    }

    // FUNCTIONS FOR MAKING IHMC MESSAGES
    void makeIHMCArmTrajectoryMessage(dynacore::Vector q_joints,
                                      controller_msgs::ArmTrajectoryMessage& arm_msg,
                                      int robot_side,
                                      IHMCMessageParameters msg_params) {
        // set sequence id, robot side, and force execution
        arm_msg.sequence_id = msg_params.sequence_id;
        arm_msg.robot_side = robot_side;
        arm_msg.force_execution = msg_params.arm_params.force_execution;

        // construct and set JointspaceTrajectoryMessage for arm
        makeIHMCJointspaceTrajectoryMessage(q_joints, arm_msg.jointspace_trajectory, msg_params);

        return;
    }

    void makeIHMCChestTrajectoryMessage(dynacore::Quaternion quat,
                                        controller_msgs::ChestTrajectoryMessage& chest_msg,
                                        IHMCMessageParameters msg_params) {
        // set sequence id
        chest_msg.sequence_id = msg_params.sequence_id;

        // construct and set SO3TrajectoryMessage for chest
        makeIHMCSO3TrajectoryMessage(quat,
                                     chest_msg.so3_trajectory,
                                     msg_params.frame_params.trajectory_reference_frame_id_pelviszup,
                                     msg_params.frame_params.data_reference_frame_id_world,
                                     msg_params);

        return;
    }

    void makeIHMCFootTrajectoryMessage(dynacore::Vect3 pos,
                                       dynacore::Quaternion quat,
                                       controller_msgs::FootTrajectoryMessage& foot_msg,
                                       int robot_side,
                                       IHMCMessageParameters msg_params) {
        // set sequence id and robot side
        foot_msg.sequence_id = msg_params.sequence_id;
        foot_msg.robot_side = robot_side;

        // construct and set SE3TrajectoryMessage for foot
        makeIHMCSE3TrajectoryMessage(pos, quat,
                                     foot_msg.se3_trajectory,
                                     msg_params.frame_params.trajectory_reference_frame_id_world,
                                     msg_params.frame_params.data_reference_frame_id_world,
                                     msg_params);

        return;
    }

    void makeIHMCFrameInformationMessage(controller_msgs::FrameInformation& frame_msg,
                                         int trajectory_reference_frame_id,
                                         int data_reference_frame_id,
                                         IHMCMessageParameters msg_params) {
        // set sequence id, trajectory reference frame, and data reference frame
        frame_msg.sequence_id = msg_params.sequence_id;
        frame_msg.trajectory_reference_frame_id = trajectory_reference_frame_id;
        frame_msg.data_reference_frame_id = data_reference_frame_id;

        return;
    }

    void makeIHMCJointspaceTrajectoryMessage(dynacore::Vector q_joints,
                                             controller_msgs::JointspaceTrajectoryMessage& js_msg,
                                             IHMCMessageParameters msg_params) {
        // set sequence id
        js_msg.sequence_id = msg_params.sequence_id;

        // construct and set queueing properties message
        makeIHMCQueueableMessage(js_msg.queueing_properties, msg_params);

        // clear vector of joint trajectory messages
        js_msg.joint_trajectory_messages.clear();

        // set trajectory for each joint
        for( int i = 0 ; i < q_joints.size() ; i++ ) {
            // construct OneDoFJointTrajectoryMessage for joint
            controller_msgs::OneDoFJointTrajectoryMessage j_msg;
            makeIHMCOneDoFJointTrajectoryMessage(q_joints[i], j_msg, msg_params);

            // add OneDoFJointTrajectoryMessage to vector
            js_msg.joint_trajectory_messages.push_back(j_msg);
        }

        return;
    }

    void makeIHMCNeckTrajectoryMessage(dynacore::Vector q_joints,
                                       controller_msgs::NeckTrajectoryMessage& neck_msg,
                                       IHMCMessageParameters msg_params) {
        // set sequence id
        neck_msg.sequence_id = msg_params.sequence_id;

        // construct and set JointspaceTrajectoryMessage for neck
        makeIHMCJointspaceTrajectoryMessage(q_joints, neck_msg.jointspace_trajectory, msg_params);

        return;
    }

    void makeIHMCOneDoFJointTrajectoryMessage(double q_joint,
                                              controller_msgs::OneDoFJointTrajectoryMessage& j_msg,
                                              IHMCMessageParameters msg_params) {
        // set sequence id and weight
        j_msg.sequence_id = msg_params.sequence_id;
        j_msg.weight = msg_params.onedof_joint_params.weight;

        // clear vector of trajectory points
        j_msg.trajectory_points.clear();

        // construct TrajectoryPoint1DMessage
        controller_msgs::TrajectoryPoint1DMessage point_msg;
        makeIHMCTrajectoryPoint1DMessage(q_joint, point_msg, msg_params);

        // add TrajectoryPoint1DMessage to vector
        j_msg.trajectory_points.push_back(point_msg);

        return;
    }

    void makeIHMCPelvisTrajectoryMessage(dynacore::Vector q_joints,
                                         controller_msgs::PelvisTrajectoryMessage& pelvis_msg,
                                         IHMCMessageParameters msg_params) {
        // set sequence id, force execution, user mode, user mode during walking
        pelvis_msg.sequence_id = msg_params.sequence_id;
        pelvis_msg.force_execution = msg_params.pelvis_params.force_execution;
        pelvis_msg.enable_user_pelvis_control = msg_params.pelvis_params.enable_user_pelvis_control;
        pelvis_msg.enable_user_pelvis_control_during_walking = msg_params.pelvis_params.enable_user_pelvis_control_during_walking;

        // get pose from given configuration
        dynacore::Vect3 pelvis_pos;
        dynacore::Quaternion pelvis_quat;
        getPelvisPose(q_joints, pelvis_pos, pelvis_quat);

        // construct and set SE3TrajectoryMessage for pelvis
        makeIHMCSE3TrajectoryMessage(pelvis_pos, pelvis_quat,
                                     pelvis_msg.se3_trajectory,
                                     msg_params.frame_params.trajectory_reference_frame_id_world,
                                     msg_params.frame_params.data_reference_frame_id_world,
                                     msg_params);

        return;
    }

    void makeIHMCQueueableMessage(controller_msgs::QueueableMessage& q_msg,
                                  IHMCMessageParameters msg_params) {
        // set sequence id, execution mode, and message id
        q_msg.sequence_id = msg_params.sequence_id;
        q_msg.execution_mode = msg_params.queueable_params.execution_mode;
        q_msg.message_id = msg_params.queueable_params.message_id;

        // if queueing messages, set previous message id
        if( msg_params.queueable_params.execution_mode == 1 ) {
            q_msg.previous_message_id = msg_params.queueable_params.previous_message_id;
        }

        // if streaming messages, set integration duration
        if( msg_params.queueable_params.execution_mode == 2 ) {
            q_msg.stream_integration_duration = msg_params.queueable_params.stream_integration_duration;
        }

        // get current time for timestamp
        auto t = std::chrono::system_clock::now();
        // set timestamp in nanoseconds when the message was created
        q_msg.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(t.time_since_epoch()).count();

        return;
    }

    void makeIHMCSE3TrajectoryMessage(dynacore::Vect3 pos, dynacore::Quaternion quat,
                                      controller_msgs::SE3TrajectoryMessage& se3_msg,
                                      int trajectory_reference_frame_id,
                                      int data_reference_frame_id,
                                      IHMCMessageParameters msg_params) {
        // set sequence id and custom control frame flag
        se3_msg.sequence_id = msg_params.sequence_id;
        se3_msg.use_custom_control_frame = msg_params.se3so3_params.use_custom_control_frame;

        // construct and set custom control frame pose (setting pose to all zeros)
        ROSMsgUtils::makeZeroPoseMessage(se3_msg.control_frame_pose);

        // construct and set queueing properties message
        makeIHMCQueueableMessage(se3_msg.queueing_properties, msg_params);

        // construct and set frame information
        makeIHMCFrameInformationMessage(se3_msg.frame_information,
                                        trajectory_reference_frame_id,
                                        data_reference_frame_id,
                                        msg_params);

        // construct and set selection matrices
        makeIHMCSelectionMatrix3DMessage(se3_msg.angular_selection_matrix, msg_params);
        makeIHMCSelectionMatrix3DMessage(se3_msg.linear_selection_matrix, msg_params);

        // construct and set weight matrices
        makeIHMCWeightMatrix3DMessage(se3_msg.angular_weight_matrix, msg_params);
        makeIHMCWeightMatrix3DMessage(se3_msg.linear_weight_matrix, msg_params);

        // clear vector of trajectory points
        se3_msg.taskspace_trajectory_points.clear();

        // construct SE3TrajectoryPointMessage
        controller_msgs::SE3TrajectoryPointMessage se3_point_msg;
        makeIHMCSE3TrajectoryPointMessage(pos, quat, se3_point_msg, msg_params);

        // add SE3TrajectoryPointMessage to vector
        se3_msg.taskspace_trajectory_points.push_back(se3_point_msg);

        return;
    }

    void makeIHMCSE3TrajectoryPointMessage(dynacore::Vect3 pos, dynacore::Quaternion quat,
                                           controller_msgs::SE3TrajectoryPointMessage& se3_point_msg,
                                           IHMCMessageParameters msg_params) {
        // set sequence id and time
        se3_point_msg.sequence_id = msg_params.sequence_id;
        se3_point_msg.time = msg_params.traj_point_params.time;

        // set position based on given position
        ROSMsgUtils::makePointMessage(pos, se3_point_msg.position);

        // set orientation based on given orientation
        ROSMsgUtils::makeQuaternionMessage(quat, se3_point_msg.orientation);

        // set linear and angular velocity to zero
        ROSMsgUtils::makeZeroVector3Message(se3_point_msg.linear_velocity);
        ROSMsgUtils::makeZeroVector3Message(se3_point_msg.angular_velocity);

        return;
    }

    void makeIHMCSelectionMatrix3DMessage(controller_msgs::SelectionMatrix3DMessage& selmat_msg,
                                          IHMCMessageParameters msg_params) {
        // set sequence id, selection frame id, and axes to select
        selmat_msg.sequence_id = msg_params.sequence_id;
        selmat_msg.selection_frame_id = msg_params.selection_matrix_params.selection_frame_id;
        selmat_msg.x_selected = msg_params.selection_matrix_params.x_selected;
        selmat_msg.y_selected = msg_params.selection_matrix_params.y_selected;
        selmat_msg.z_selected = msg_params.selection_matrix_params.z_selected;

        return;
    }

    void makeIHMCSO3TrajectoryMessage(dynacore::Quaternion quat,
                                      controller_msgs::SO3TrajectoryMessage& so3_msg,
                                      int trajectory_reference_frame_id,
                                      int data_reference_frame_id,
                                      IHMCMessageParameters msg_params) {
        // set sequence id and custom control frame flag
        so3_msg.sequence_id = msg_params.sequence_id;
        so3_msg.use_custom_control_frame = msg_params.se3so3_params.use_custom_control_frame;

        // construct and set custom control frame pose (setting pose to all zeros)
        ROSMsgUtils::makeZeroPoseMessage(so3_msg.control_frame_pose);

        // construct and set queueing properties message
        makeIHMCQueueableMessage(so3_msg.queueing_properties, msg_params);

        // construct and set frame information
        makeIHMCFrameInformationMessage(so3_msg.frame_information,
                                        trajectory_reference_frame_id,
                                        data_reference_frame_id,
                                        msg_params);

        // construct and set selection matrix
        makeIHMCSelectionMatrix3DMessage(so3_msg.selection_matrix, msg_params);

        // construct and set weight matrix
        makeIHMCWeightMatrix3DMessage(so3_msg.weight_matrix, msg_params);

        // clear vector of trajectory points
        so3_msg.taskspace_trajectory_points.clear();

        // construct SO3TrajectoryPointMessage
        controller_msgs::SO3TrajectoryPointMessage so3_point_msg;
        makeIHMCSO3TrajectoryPointMessage(quat, so3_point_msg, msg_params);

        // add SO3TrajectoryPointMessage to vector
        so3_msg.taskspace_trajectory_points.push_back(so3_point_msg);

        return;
    }

    void makeIHMCSO3TrajectoryPointMessage(dynacore::Quaternion quat,
                                           controller_msgs::SO3TrajectoryPointMessage& so3_point_msg,
                                           IHMCMessageParameters msg_params) {
        // set sequence id and time
        so3_point_msg.sequence_id = msg_params.sequence_id;
        so3_point_msg.time = msg_params.traj_point_params.time;

        // set quaternion based on given orientation
        ROSMsgUtils::makeQuaternionMessage(quat, so3_point_msg.orientation);

        // set angular velocity to zero
        ROSMsgUtils::makeZeroVector3Message(so3_point_msg.angular_velocity);

        return;
    }

    void makeIHMCSpineTrajectoryMessage(dynacore::Vector q_joints,
                                        controller_msgs::SpineTrajectoryMessage& spine_msg,
                                        IHMCMessageParameters msg_params) {
        // set sequence id
        spine_msg.sequence_id = msg_params.sequence_id;

        // construct and set JointspaceTrajectoryMessage for spine
        makeIHMCJointspaceTrajectoryMessage(q_joints, spine_msg.jointspace_trajectory, msg_params);

        return;
    }

    void makeIHMCTrajectoryPoint1DMessage(double q_joint,
                                          controller_msgs::TrajectoryPoint1DMessage& point_msg,
                                          IHMCMessageParameters msg_params) {
        // set sequence id and time
        point_msg.sequence_id = msg_params.sequence_id;
        point_msg.time = msg_params.traj_point_params.time;

        // set desired position based on input
        point_msg.position = q_joint;

        // set desired velocity to 0
        point_msg.velocity = 0.0;

        return;
    }

    void makeIHMCWeightMatrix3DMessage(controller_msgs::WeightMatrix3DMessage& wmat_msg,
                                       IHMCMessageParameters msg_params) {
        // set sequence id, weight frame id, and axis weights
        wmat_msg.sequence_id = msg_params.sequence_id;
        wmat_msg.weight_frame_id = msg_params.weight_matrix_params.weight_frame_id;
        wmat_msg.x_weight = msg_params.weight_matrix_params.x_weight;
        wmat_msg.y_weight = msg_params.weight_matrix_params.y_weight;
        wmat_msg.z_weight = msg_params.weight_matrix_params.z_weight;

        return;
    }

    void makeIHMCWholeBodyTrajectoryMessage(dynacore::Vector q,
                                            controller_msgs::WholeBodyTrajectoryMessage& wholebody_msg,
                                            IHMCMessageParameters msg_params) {
        // set sequence id
        wholebody_msg.sequence_id = msg_params.sequence_id;

        // check what links given configuration is controlling
        // we will not set whole-body message information for not controlled links
        bool control_pelvis = checkControlledLink(msg_params.controlled_links, valkyrie_link::pelvis);
        bool control_chest = checkControlledLink(msg_params.controlled_links, valkyrie_link::torso);
        bool control_rfoot = checkControlledLink(msg_params.controlled_links, valkyrie_link::rightCOP_Frame);
        bool control_lfoot = checkControlledLink(msg_params.controlled_links, valkyrie_link::leftCOP_Frame);
        bool control_rarm = checkControlledLink(msg_params.controlled_links, valkyrie_link::rightPalm);
        bool control_larm = checkControlledLink(msg_params.controlled_links, valkyrie_link::leftPalm);
        bool control_neck = checkControlledLink(msg_params.controlled_links, valkyrie_link::head);
        // not used:
        // bool control_rfoot = checkControlledLink(msg_params.controlled_links, valkyrie_link::rightFoot);
        // bool control_lfoot = checkControlledLink(msg_params.controlled_links, valkyrie_link::leftFoot);

        // HAND TRAJECTORIES (not needed)
        // do not set trajectory for left hand: wholebody_msg.left_hand_trajectory_message
        // do not set trajectory for right hand: wholebody_msg.right_hand_trajectory_message

        // ARM TRAJECTORIES
        if( control_larm ) {
            // get relevant joint indices for left arm
            std::vector<int> larm_joint_indices;
            getRelevantJointIndicesLeftArm(larm_joint_indices);
            // get relevant configuration values for left arm
            dynacore::Vector q_larm;
            selectRelevantJointsConfiguration(q, larm_joint_indices, q_larm);
            // construct and set arm message for left arm
            makeIHMCArmTrajectoryMessage(q_larm,
                                         wholebody_msg.left_arm_trajectory_message,
                                         0, msg_params);
        }

        if( control_rarm ) {
            // get relevant joint indices for right arm
            std::vector<int> rarm_joint_indices;
            getRelevantJointIndicesRightArm(rarm_joint_indices);
            // get relevant configuration values for right arm
            dynacore::Vector q_rarm;
            selectRelevantJointsConfiguration(q, rarm_joint_indices, q_rarm);
            // construct and set arm message for right arm
            makeIHMCArmTrajectoryMessage(q_rarm,
                                         wholebody_msg.right_arm_trajectory_message,
                                         1, msg_params);
        }

        // CHEST TRAJECTORY
        if( control_chest ) {
            // get orientation of chest induced by configuration
            dynacore::Quaternion chest_quat;
            getChestOrientation(q, chest_quat);
            // construct and set chest message
            makeIHMCChestTrajectoryMessage(chest_quat, wholebody_msg.chest_trajectory_message, msg_params);
        }

        // SPINE TRAJECTORY
        /*
         * NOTE: spine trajectories work well in sim, but not on real robot;
         * the code below has been tested in sim and works,
         * but is commented out since it is unreliable in practice
         */
        /*
        if( control_chest ) {
            // get relevant joint indices for spine
            std::vector<int> torso_joint_indices;
            getRelevantJointIndicesTorso(torso_joint_indices);
            // get relevant configuration values for spine
            dynacore::Vector q_spine;
            selectRelevantJointsConfiguration(q, torso_joint_indices, q_spine);
            // construct and set spine message
            makeIHMCSpineTrajectoryMessage(q_spine, wholebody_msg.spine_trajectory_message, msg_params);
        }
        */

        // PELVIS TRAJECTORY
        if( control_pelvis ) {
            // get relevant joint indices for pelvis
            std::vector<int> pelvis_joint_indices;
            getRelevantJointIndicesPelvis(pelvis_joint_indices);
            // get relevant configuration values for pelvis
            dynacore::Vector q_pelvis;
            selectRelevantJointsConfiguration(q, pelvis_joint_indices, q_pelvis);
            // construct and set pelvis message
            makeIHMCPelvisTrajectoryMessage(q_pelvis, wholebody_msg.pelvis_trajectory_message, msg_params);
        }

        // FOOT TRAJECTORIES
        /*
         * NOTE: foot trajectories will be complicated to send because
         * IHMC interface has safety features to prevent moving feet when robot is already standing;
         * the code below has been tested in sim and it does seem to move the feet,
         * but not accurately due to balance issues;
         * sending foot trajectories also seems to interfere with arms,
         * so code is commented out since we will trust the robot to balance on its own
         */
        /*
        // get poses of feet induced by configuration
        dynacore::Vect3 lfoot_pos;
        dynacore::Quaternion lfoot_quat;
        dynacore::Vect3 rfoot_pos;
        dynacore::Quaternion rfoot_quat;
        getFeetPoses(q, lfoot_pos, lfoot_quat, rfoot_pos, rfoot_quat);
        if( control_lfoot ) {
            // construct and set left foot message
            makeIHMCFootTrajectoryMessage(lfoot_pos, lfoot_quat,
                                          wholebody_msg.left_foot_trajectory_message,
                                          0, msg_params);
        }
        if( control_rfoot ) {
            // construct and set right foot message
            makeIHMCFootTrajectoryMessage(rfoot_pos, rfoot_quat,
                                          wholebody_msg.left_foot_trajectory_message,
                                          1, msg_params);
        }
        */

        // NECK TRAJECTORY
        if( control_neck ) {
            // get relevant joint indices for neck
            std::vector<int> neck_joint_indices;
            getRelevantJointIndicesNeck(neck_joint_indices);
            // get relevant configuration values for neck
            dynacore::Vector q_neck;
            selectRelevantJointsConfiguration(q, neck_joint_indices, q_neck);
            // construct and set neck message
            makeIHMCNeckTrajectoryMessage(q_neck, wholebody_msg.neck_trajectory_message, msg_params);
        }

        // HEAD TRAJECTORY
        // do not set trajectory for head: wholebody_msg.head_trajectory_message

        return;
    }

    void makeIHMCHomeLeftArmMessage(controller_msgs::GoHomeMessage& go_home_msg,
                                    IHMCMessageParameters msg_params)
    {
        // set body part, robot side, and trajectory time
        go_home_msg.humanoid_body_part = go_home_msg.HUMANOID_BODY_PART_ARM;
        go_home_msg.robot_side = go_home_msg.ROBOT_SIDE_LEFT;
        go_home_msg.trajectory_time = msg_params.go_home_params.trajectory_time;

        return;
    }

    void makeIHMCHomeRightArmMessage(controller_msgs::GoHomeMessage& go_home_msg,
                                     IHMCMessageParameters msg_params)
    {
        // set body part, robot side, and trajectory time
        go_home_msg.humanoid_body_part = go_home_msg.HUMANOID_BODY_PART_ARM;
        go_home_msg.robot_side = go_home_msg.ROBOT_SIDE_RIGHT;
        go_home_msg.trajectory_time = msg_params.go_home_params.trajectory_time;

        return;
    }

    void makeIHMCHomeChestMessage(controller_msgs::GoHomeMessage& go_home_msg,
                                  IHMCMessageParameters msg_params)
    {
        // set body part and trajectory time
        go_home_msg.humanoid_body_part = go_home_msg.HUMANOID_BODY_PART_CHEST;
        go_home_msg.trajectory_time = msg_params.go_home_params.trajectory_time;

        return;
    }

    void makeIHMCHomePelvisMessage(controller_msgs::GoHomeMessage& go_home_msg,
                                   IHMCMessageParameters msg_params)
    {
        // set body part and trajectory time
        go_home_msg.humanoid_body_part = go_home_msg.HUMANOID_BODY_PART_PELVIS;
        go_home_msg.trajectory_time = msg_params.go_home_params.trajectory_time;

        return;
    }

    // HELPER FUNCTIONS
    void selectRelevantJointsConfiguration(dynacore::Vector q,
                                           std::vector<int> joint_indices,
                                           dynacore::Vector& q_joints) {
        // resize and clear relevant joint configuration vector
        q_joints.resize(joint_indices.size());
        q_joints.setZero();

        // push back relevant joint positions
        for( int i = 0 ; i < joint_indices.size() ; i++ ) {
            // check for special index -1
            if( joint_indices[i] == -1 ) {
                // special index -1 indicates that joint is not included in valkyrie definition,
                // but is needed in the wholebody message
                // set joint position to 0
                q_joints[i] = 0.0;
            }
            else {
                // joint position exists in valkyrie definition
                // set based on given configuration
                q_joints[i] = q[joint_indices[i]];
            }
        }

        return;
    }

    void getRelevantJointIndicesPelvis(std::vector<int>& joint_indices) {
        // clear joint index vector
        joint_indices.clear();

        // push back joints for pelvis [x, y, z, rx, ry, rz, rw]
        joint_indices.push_back(valkyrie_joint::virtual_X);
        joint_indices.push_back(valkyrie_joint::virtual_Y);
        joint_indices.push_back(valkyrie_joint::virtual_Z);
        joint_indices.push_back(valkyrie_joint::virtual_Rx);
        joint_indices.push_back(valkyrie_joint::virtual_Ry);
        joint_indices.push_back(valkyrie_joint::virtual_Rz);
        joint_indices.push_back(valkyrie_joint::virtual_Rw);

        return;
    }

    void getRelevantJointIndicesLeftLeg(std::vector<int>& joint_indices) {
        // clear joint index vector
        joint_indices.clear();

        // push back joints for left leg [hipYaw, hipRoll, hipPitch, kneePitch, anklePitch, ankleRoll]
        joint_indices.push_back(valkyrie_joint::leftHipYaw);
        joint_indices.push_back(valkyrie_joint::leftHipRoll);
        joint_indices.push_back(valkyrie_joint::leftHipPitch);
        joint_indices.push_back(valkyrie_joint::leftKneePitch);
        joint_indices.push_back(valkyrie_joint::leftAnklePitch);
        joint_indices.push_back(valkyrie_joint::leftAnkleRoll);

        return;
    }

    void getRelevantJointIndicesRightLeg(std::vector<int>& joint_indices) {
        // clear joint index vector
        joint_indices.clear();

        // push back joints for right leg [hipYaw, hipRoll, hipPitch, kneePitch, anklePitch, ankleRoll]
        joint_indices.push_back(valkyrie_joint::rightHipYaw);
        joint_indices.push_back(valkyrie_joint::rightHipRoll);
        joint_indices.push_back(valkyrie_joint::rightHipPitch);
        joint_indices.push_back(valkyrie_joint::rightKneePitch);
        joint_indices.push_back(valkyrie_joint::rightAnklePitch);
        joint_indices.push_back(valkyrie_joint::rightAnkleRoll);

        return;
    }

    void getRelevantJointIndicesTorso(std::vector<int>& joint_indices) {
        // clear joint index vector
        joint_indices.clear();

        // push back joints for torso [yaw, pitch, roll]
        joint_indices.push_back(valkyrie_joint::torsoYaw);
        joint_indices.push_back(valkyrie_joint::torsoPitch);
        joint_indices.push_back(valkyrie_joint::torsoRoll);

        return;
    }

    void getRelevantJointIndicesLeftArm(std::vector<int>& joint_indices) {
        // clear joint index vector
        joint_indices.clear();

        // push back joints for left arm [shoulderPitch, shoulderRoll, shoulderYaw, elbowPitch, forearmYaw]
        joint_indices.push_back(valkyrie_joint::leftShoulderPitch);
        joint_indices.push_back(valkyrie_joint::leftShoulderRoll);
        joint_indices.push_back(valkyrie_joint::leftShoulderYaw);
        joint_indices.push_back(valkyrie_joint::leftElbowPitch);
        joint_indices.push_back(valkyrie_joint::leftForearmYaw);

        // push back special joint index for left wrist; not included in valkyrie definition
        joint_indices.push_back(-1); // leftWristRoll
        joint_indices.push_back(-1); // leftWristPitch

        return;
    }

    void getRelevantJointIndicesNeck(std::vector<int>& joint_indices) {
        // clear joint index vector
        joint_indices.clear();

        // push back joints for neck [lowerPitch, yaw, upperPitch]
        joint_indices.push_back(valkyrie_joint::lowerNeckPitch);
        joint_indices.push_back(valkyrie_joint::neckYaw);
        joint_indices.push_back(valkyrie_joint::upperNeckPitch);

        return;
    }

    void getRelevantJointIndicesRightArm(std::vector<int>& joint_indices) {
    // clear joint index vector
        joint_indices.clear();

        // push back joints for right arm [shoulderPitch, shoulderRoll, shoulderYaw, elbowPitch, forearmYaw]
        joint_indices.push_back(valkyrie_joint::rightShoulderPitch);
        joint_indices.push_back(valkyrie_joint::rightShoulderRoll);
        joint_indices.push_back(valkyrie_joint::rightShoulderYaw);
        joint_indices.push_back(valkyrie_joint::rightElbowPitch);
        joint_indices.push_back(valkyrie_joint::rightForearmYaw);

        // push back special joint index for right wrist; not included in valkyrie definition
        joint_indices.push_back(-1); // rightWristRoll
        joint_indices.push_back(-1); // rightWristPitch

        return;
    }

    void getChestOrientation(dynacore::Vector q, dynacore::Quaternion& chest_quat) {
        // construct robot model
        std::shared_ptr<Valkyrie_Model> robot_model(new Valkyrie_Model);

        // initialize zero velocity vector
        dynacore::Vector qdot;
        qdot.resize(valkyrie::num_qdot);
        qdot.setZero();

        // update system to reflect joint configuration
        robot_model->UpdateSystem(q, qdot);

        // get orientation of chest based on joint configuration
        robot_model->getOri(valkyrie_link::torso, chest_quat);

        return;
    }

    void getPelvisPose(dynacore::Vector q_joints,
                       dynacore::Vect3& pelvis_pos, dynacore::Quaternion& pelvis_quat) {
        // get position of pelvis based on given configuration
        pelvis_pos[0] = q_joints[0];
        pelvis_pos[1] = q_joints[1];
        pelvis_pos[2] = q_joints[2];
        // set orientation of pelvis based on given configuration
        pelvis_quat.x() = q_joints[3];
        pelvis_quat.y() = q_joints[4];
        pelvis_quat.z() = q_joints[5];
        pelvis_quat.w() = q_joints[6];

        return;
    }

    void getFeetPoses(dynacore::Vector q,
                      dynacore::Vect3& lfoot_pos, dynacore::Quaternion& lfoot_quat,
                      dynacore::Vect3& rfoot_pos, dynacore::Quaternion& rfoot_quat) {
        // construct robot model
        std::shared_ptr<Valkyrie_Model> robot_model(new Valkyrie_Model);

        // initialize zero velocity vector
        dynacore::Vector qdot;
        qdot.resize(valkyrie::num_qdot);
        qdot.setZero();

        // update system to reflect joint configuration
        robot_model->UpdateSystem(q, qdot);

        // get pose of left foot based on joint configuration
        robot_model->getPos(valkyrie_link::leftCOP_Frame, lfoot_pos);
        robot_model->getOri(valkyrie_link::leftCOP_Frame, lfoot_quat);

        // get pose of right foot based on joint configuration
        robot_model->getPos(valkyrie_link::rightCOP_Frame, rfoot_pos);
        robot_model->getOri(valkyrie_link::rightCOP_Frame, rfoot_quat);

        return;
    }

    bool checkControlledLink(std::vector<int> controlled_links, int link_id) {
        // check if link id is in vector
        std::vector<int>::iterator it;
        it = std::find(controlled_links.begin(), controlled_links.end(), link_id);

        return (it != controlled_links.end());
    }

} // end namespace IHMCMsgUtilities
