package bgu.spl.net.impl.rci;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicInteger;

public class Info {

        // map topic to queue of users that subscribe to this topic
        private ConcurrentHashMap<String, ConcurrentLinkedQueue<User>> topics;
        // // map connectionId to user
        private ConcurrentHashMap<Integer, User> connectionToUser;
        private ConcurrentHashMap<String, User> usernameToUser;
        private ConcurrentHashMap<Integer, User> subIdToUser;
        private ConcurrentHashMap<String, ConcurrentLinkedQueue<Integer>> topicToSubIds;
        private AtomicInteger mId; // message id
        private static volatile Info instance;

        private Info() {
                topics = new ConcurrentHashMap<>();
                connectionToUser = new ConcurrentHashMap<>();
                subIdToUser = new ConcurrentHashMap<>();
                mId = new AtomicInteger();
                usernameToUser = new ConcurrentHashMap<>();
                topicToSubIds = new ConcurrentHashMap<>();

        }

        public static Info getInstance() {
                if (instance == null) {
                        synchronized (Info.class) { // synchronized because we are creating instance in a multi-threaded
                                                    // environment
                                if (instance == null) {
                                        instance = new Info();
                                }
                        }
                }
                return instance;
        }

        public boolean logIn(User user, int connectionId) {

                String username = user.getUserName();
                connectionToUser.put(connectionId, user);
                usernameToUser.put(username, user);
                user.Connect();

                return connectionToUser.contains(user) & usernameToUser.contains(user) & user.isActive();
        }

        public boolean topicExist(String topic) {
                return topics.containsKey(topic);
        }

        public void addToSub(Integer subId, User user, String topic) {

                ConcurrentLinkedQueue<User> queue1;
                ConcurrentLinkedQueue<Integer> queue2;

                subIdToUser.put(subId, user);
                if (!topics.containsKey(topic)) {
                        queue1 = new ConcurrentLinkedQueue<User>();
                        queue1.add(user);
                } else {
                        queue1 = topics.remove(topic);
                        queue1.add(user);
                }

                topics.putIfAbsent(topic, queue1);

                if (!topicToSubIds.containsKey(topic)) {
                        queue2 = new ConcurrentLinkedQueue<Integer>();
                        queue2.add(subId);
                } else {
                        queue2 = topicToSubIds.remove(topic);
                        queue2.add(subId);
                }
                topicToSubIds.putIfAbsent(topic, queue2);

                user.addSub(subId, topic);

        }

        public void removeFromSub(Integer subId, String topic) {
                User user = subIdToUser.get(subId); // the user that subscribe using this subId

                if (user == null) {
                        return;
                }
                if (topics.get(topic) != null && topics.get(topic).contains(user)) { // topic exist & user sub to topic
                        topics.get(topic).remove(user);
                }

                if (topicToSubIds.get(topic) != null && topicToSubIds.get(topic).contains(subId)) { // topic exist &
                                                                                                    // user sub to topic
                        topicToSubIds.get(topic).remove(subId);
                }

                user.removeSub(subId, topic);
        }

        public ConcurrentLinkedQueue<User> getTopicSubs(String topic) {
                if (topics.containsKey(topic)) {
                        return topics.get(topic);
                }
                return null; // topic doesnt exist
        }

        public String getTopics() {
                String topicsNames = "Games:\n";
                for (String topic : topics.keySet()) {
                        topicsNames += topic + "\n";
                }
                return topicsNames;
        }

        public User getUserById(int connectionId) {

                if (connectionToUser.containsKey(connectionId)) {
                        return connectionToUser.get(connectionId);
                }
                return null; // there is no connection to user with connectionId

        }

        public User getUserByUsername(String username) {

                if (usernameToUser.containsKey(username)) {
                        return usernameToUser.get(username);

                }
                return null; // user with this username not exist

        }

        public User getUserBySubId(Integer subId) {

                if (subIdToUser.containsKey(subId)) {
                        return subIdToUser.get(subId);

                }
                return null; // user with this username not exist

        }

        public ConcurrentLinkedQueue<Integer> getTopicSubIds(String topic) {
                if (topicToSubIds.containsKey(topic)) {
                        return topicToSubIds.get(topic);
                }
                return null;
        }

        public void disconnect(int connectionId) {
                connectionToUser.remove(connectionId);
        }

        public Integer getMId() {
                Integer msgId = mId.incrementAndGet();
                return msgId;
        }

}